#include <utility>

#include "Scenes/LevelScene.h"

#include "Core/HID/Input.h"
#include "Core/RGBA.h"
#include "Core/Time.h"
#include "Core/TimerManager.h"
#include "Gameplay/Components/CameraComponent.h"
#include "Gameplay/EntityManager.h"

#include "Levels/LevelGenerator.h"

#include "Core/Settings/SettingsManager.h"

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Levels/NavigationGraph.h"
#include "SceneGraph/SceneGraph.h"

#include "ai/AIComponent.h"
#include "ai/Path.h"
#include "ai/SteeringBehaviors.h"
#include "ai/TrafficCar.h"

#include "UI/GameplayOverlay.h"

#include "Managers/AudioManager.h"
#include "Managers/GameplayEventsManager.h"
#include "Managers/LevelTaskManager.h"
#include "Managers/ScoreManager.h"
#include "Managers/TaskManager.h"

#include "UI/FinishScreen.h"
#include "UI/PauseMenu.h"
#include "UI/StartLevelCountdown.h"

LevelScene::LevelScene(std::string path) : levelPath(std::move(path)) {}

LevelScene::~LevelScene() = default;

void LevelScene::Load() {
    LoadLevel();
    InitializeLevelTaskManager();

    gameplayOverlay = mlg::EntityManager::SpawnEntity<GameplayOverlay>(
                              "Overlay", false, mlg::SceneGraph::GetRoot(),
                              levelTaskManager)
                              .lock();

    navigationGraph = std::make_shared<NavigationGraph>(levelPath);
    scoreManager = std::make_unique<ScoreManager>();
    pauseMenu = mlg::EntityManager::SpawnEntity<PauseMenu>(
            "PauseMenu", false, mlg::SceneGraph::GetRoot());
    finishScreen = mlg::EntityManager::SpawnEntity<FinishScreen>(
                           "FinishScreen", false, mlg::SceneGraph::GetRoot())
                           .lock();

    gameplayEventsManager = std::make_unique<GameplayEventsManager>(levelPath);

    audioManager = mlg::EntityManager::SpawnEntity<AudioManager>(
                           "AudioManager", false, mlg::SceneGraph::GetRoot())
                           .lock();

    SpawnTraffic();
    SetTimeLimit();

    levelCountdown = mlg::EntityManager::SpawnEntity<StartLevelCountdown>(
                             "StartLevelCountdown", false, mlg::SceneGraph::GetRoot())
                             .lock();

    // TODO: Remove me
    gameplayOverlay->SetChat(fmt::format(
            "Welcome to {}, useless piece of meat!", levelName));
}

void LevelScene::Update() {
    HandlePauseGame();
    levelTaskManager->GetTaskManager().Update();

    float timeLeft = mlg::TimerManager::Get()->GetTimerRemainingTime(timeLimitTimer);
    gameplayOverlay->SetClock(timeLeft);
    audioManager->SetTimeLeft(timeLeft);


#ifdef DEBUG
    if (mlg::Input::IsActionJustPressed("debug_event"))
        gameplayEventsManager->TriggerEvent();

    // Debug: Navigation graph
    if (mlg::SettingsManager::Get<bool>(
                mlg::SettingsType::Debug, "showNavigation") &&
        navigationGraph != nullptr) {

        navigationGraph->DrawNodes();
    }

    // IMGUI: Task manager
    ImGui::Begin("Task manager");
    {
        ImGui::Text("Score: %i", scoreManager->GetScore());
        ImGui::Separator();

        auto activeTasks = levelTaskManager->GetTaskManager().GetActiveTasks();
        for (const auto& task : activeTasks) {
            ImGui::Text("Task: %s", task.productId.c_str());
            ImGui::Text("Time left: %f", task.time);
            ImGui::Text("Reward: %i", task.reward);
            ImGui::Text("Bonus: %i", task.bonus);
            ImGui::Separator();
        }
    }
    ImGui::End();

#endif
}

void LevelScene::HandlePauseGame() {

    if (mlg::Input::IsActionJustPressed("pause") &&
        mlg::TimerManager::Get()->GetTimerRemainingTime(timeLimitTimer) > 0.0f) {
        bool isGamePaused = mlg::Time::IsGamePaused();
        mlg::Time::PauseGame(!isGamePaused);
        pauseMenu.lock()->SetVisible(!isGamePaused);
    }
}

const std::shared_ptr<NavigationGraph>& LevelScene::GetNavigationGraph() const {
    return navigationGraph;
}

LevelTaskManager* LevelScene::GetLevelTaskManager() {
    return levelTaskManager.get();
}

TaskManager* LevelScene::GetTaskManager() {
    return &levelTaskManager->GetTaskManager();
}

ScoreManager* LevelScene::GetScoreManager() {
    return scoreManager.get();
}

const std::string& LevelScene::GetLevelName() const {
    return levelName;
}

GameplayEventsManager& LevelScene::GetGameplayEventsManager() {
    return *gameplayEventsManager;
}

StartLevelCountdown& LevelScene::GetLevelCountdown() {
    return *levelCountdown;
}

void LevelScene::InitializeLevelTaskManager() {
    levelTaskManager = std::make_unique<LevelTaskManager>();
    levelTaskManager->GetTaskManager().OnTaskFinished.append(
            [this](const TaskData& taskData) {
                int reward = taskData.reward;

                if (taskData.time > 0.0f) {
                    reward += taskData.bonus;
                }

                scoreManager->AddScore(reward);
                gameplayOverlay->SetScore(scoreManager->GetScore());
                gameplayEventsManager->TriggerEvent();
            });
    levelTaskManager->GetTaskManager().OnProductSold.append(
            [this](int price) {

                scoreManager->AddScore(price);
                gameplayOverlay->SetScore(scoreManager->GetScore());

                // TODO: transfer to chat manager
                gameplayOverlay->SetChat(fmt::format(
                        "You sold product for {}$, useless piece of meat! You are courier, not a merchant!",
                        price));
            });

    std::vector<TaskData> tasks = mlg::LevelGenerator::GetTasks(levelPath);
    for (const auto& task : tasks) {
        GetTaskManager()->AddTaskToPool(task);
    }

    levelTaskManager->StartNewTaskLogic();
}

void LevelScene::SpawnTraffic() {
    // TODO: AI cars should be spawned on the road
    mlg::LevelGenerator::TrafficData trafficData = mlg::LevelGenerator::LoadTrafficData(levelPath);

    uint32_t i = 0;

    for (const auto& node : navigationGraph->GetAllNodes()) {
        if (i >= trafficData.numberOfAgents)
            break;

        TrafficCarData aiCarData = {static_cast<int>(i), mlg::RGBA::black, "res/config/cars/traffic.json"};
        auto aiCar =
                mlg::EntityManager::SpawnEntity<TrafficCar>(
                        "TrafficCar", false, mlg::SceneGraph::GetRoot(), aiCarData);

        auto aiCarRigidbody =
                aiCar.lock()->GetComponentByName<mlg::RigidbodyComponent>("Rigidbody");

        aiCarRigidbody.lock()->SetPosition(node->position);
        i++;
    }
}

void LevelScene::LoadLevel() {
    auto cameraEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("Camera", false, mlg::SceneGraph::GetRoot());
    auto cameraComponent = cameraEntity.lock()->AddComponent<mlg::CameraComponent>("CameraComponent");

    mlg::LevelGenerator::LoadMap(levelPath);
    mlg::LevelGenerator::SpawnGroundAndWater(levelPath);
    mlg::LevelGenerator::SetCityBounds(levelPath);
    mlg::LevelGenerator::LoadCameraSettings(levelPath, *cameraComponent.lock());
    mlg::LevelGenerator::SpawnPlayers(levelPath);
    levelName = mlg::LevelGenerator::LoadLevelName(levelPath);
}

void LevelScene::SetTimeLimit() {
    float timeLimit = mlg::LevelGenerator::LoadLevelTimeLimit(levelPath);

    if (timeLimit > 0.f) {
        timeLimitTimer = mlg::TimerManager::Get()->SetTimer(
                timeLimit,
                false,
                [this]() {
                    mlg::Time::PauseGame(true);
                    finishScreen->SetScore(scoreManager->GetScore(), levelName);
                    finishScreen->SetVisible(true);
                    OnLevelFinished();
                    mlg::Time::PauseGame(true);
                });
    }
}
