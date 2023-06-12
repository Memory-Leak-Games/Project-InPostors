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

#include "Managers/LevelTaskManager.h"
#include "Managers/ScoreManager.h"
#include "Managers/TaskManager.h"
#include "Managers/GameplayEventsManager.h"

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

    SpawnTraffic();
    LoadSounds();
    SetTimeLimit();

    mlg::EntityManager::SpawnEntity<StartLevelCountdown>(
        "StartLevelCountdown", false, mlg::SceneGraph::GetRoot());

    // TODO: Remove me
    gameplayOverlay->SetChat(fmt::format(
            "Welcome to {}, useless piece of meat!", levelName));
}

void LevelScene::Update() {
    HandlePauseGame();
    levelTaskManager->GetTaskManager().Update();

    float timeLeft = mlg::TimerManager::Get()->GetTimerRemainingTime(timeLimitTimer);
    gameplayOverlay->SetClock(timeLeft);

    if (timeLeft < 11 && timeLeft > 0) {
        if (canPlaySound) {
            auto enableSoundLambda = [this]() {
                canPlaySound = true;
            };

            clockCountdownSound->Play();
            canPlaySound = false;
            mlg::TimerManager::Get()->SetTimer(1.f, false, enableSoundLambda);
        }
    }

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

        TrafficCarData aiCarData = {0, mlg::RGBA::white};
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

void LevelScene::LoadSounds() {
    cityAmbientSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/music/city_ambient.mp3");
    cityAmbientSound->PlayBackgroundMusic(2.f);
    clockCountdownSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/clock_countdown.wav");
    boxingBellSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/boxing_bell.mp3");
}

void LevelScene::SetTimeLimit() {
    float timeLimit = mlg::LevelGenerator::LoadLevelTimeLimit(levelPath);

    if (timeLimit > 0.f) {
        timeLimitTimer = mlg::TimerManager::Get()->SetTimer(
                timeLimit,
                false,
                [this]() {
                    boxingBellSound->Play(2.f);
                    mlg::Time::PauseGame(true);
                    finishScreen->SetScore(scoreManager->GetScore(), levelName);
                    finishScreen->SetVisible(true);
                    mlg::Time::PauseGame(true);
                });
    }
}
