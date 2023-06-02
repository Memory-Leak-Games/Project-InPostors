#include "Scenes/LevelScene.h"

#include "Core/HID/Input.h"
#include "Core/RGBA.h"
#include "Core/Time.h"
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

LevelScene::LevelScene(const std::string& path) : levelPath(path) {}

void LevelScene::Load() {
    LoadLevel();

    taskManager = std::make_unique<TaskManager>();
    navigationGraph = std::make_shared<NavigationGraph>(levelPath);

    SpawnTraffic();
    LoadSounds();
}

void LevelScene::Update() {
    HandlePauseGame();
    taskManager->Update();

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
        auto activeTasks = taskManager->GetActiveTasks();
        for (const auto& task : activeTasks) {
            ImGui::Text("Task: %s", task.productId.c_str());
            ImGui::Text("Time left: %f", task.time);
            ImGui::Text("Reward: %f", task.reward);
            ImGui::Text("Bonus: %f", task.bonus);
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
}

void LevelScene::LoadSounds() {
    cityAmbientSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/music/city_ambient.mp3");
    cityAmbientSound->PlayBackgroundMusic(2.f);
}
