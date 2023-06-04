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

#include "ScoreManager.h"
#include "TaskManager.h"

LevelScene::LevelScene(std::string path) : levelPath(std::move(path)) {}

LevelScene::~LevelScene() = default;

void LevelScene::Load() {
    LoadLevel();

    taskManager = std::make_unique<TaskManager>();
    scoreManager = std::make_unique<ScoreManager>();
    taskManager->OnTaskFinished.append([this](const TaskData& taskData) {
        int reward = taskData.reward;

        if (taskData.time > 0.0f) {
            reward += taskData.bonus;
        }

        scoreManager->AddScore(reward);
        gameplayOverlay->SetScore(scoreManager->GetScore());
    });

    gameplayOverlay = mlg::EntityManager::SpawnEntity<GameplayOverlay>(
                              "Overlay",
                              false,
                              mlg::SceneGraph::GetRoot())
                              .lock();

    navigationGraph = std::make_shared<NavigationGraph>(levelPath);

    SpawnTraffic();
    LoadSounds();

    SetTimeLimit();

    // TODO: Remove me
    gameplayOverlay->SetChat(fmt::format(
            "Welcome to {}, useless piece of meat!", levelName));
}

void LevelScene::Update() {
    HandlePauseGame();
    taskManager->Update();

    float timeLeft = mlg::TimerManager::Get()->GetTimerRemainingTime(timeLimitTimer);
    gameplayOverlay->SetClock(timeLeft);

#ifdef DEBUG
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

        auto activeTasks = taskManager->GetActiveTasks();
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
    if (mlg::Input::IsActionJustPressed("pause")) {
        bool isGamePaused = mlg::Time::IsGamePaused();
        mlg::Time::PauseGame(!isGamePaused);
    }
}

const std::shared_ptr<NavigationGraph>& LevelScene::GetNavigationGraph() const {
    return navigationGraph;
}

TaskManager* LevelScene::GetTaskManager() {
    return taskManager.get();
}

ScoreManager* LevelScene::GetScoreManager() {
    return scoreManager.get();
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
    mlg::LevelGenerator::SpawnGround(levelPath);
    mlg::LevelGenerator::SetCityBounds(levelPath);
    mlg::LevelGenerator::LoadCameraSettings(levelPath, *cameraComponent.lock());
    mlg::LevelGenerator::SpawnPlayers(levelPath);
    levelName = mlg::LevelGenerator::LoadLevelName(levelPath);
}

void LevelScene::LoadSounds() {
    cityAmbientSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/music/city_ambient.mp3");
    cityAmbientSound->PlayBackgroundMusic(2.f);
}

void LevelScene::SetTimeLimit() {
    float timeLimit = mlg::LevelGenerator::LoadLevelTimeLimit(levelPath);

    if (timeLimit > 0.f) {
        timeLimitTimer = mlg::TimerManager::Get()->SetTimer(
                timeLimit,
                false,
                [this]() {
                    mlg::Time::PauseGame(true);
                    SPDLOG_INFO("GameOver, Score: {}", scoreManager->GetScore());
                });
    }
}
