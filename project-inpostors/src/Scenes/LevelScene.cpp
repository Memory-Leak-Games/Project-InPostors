#include "Scenes/LevelScene.h"

#include "Core/RGBA.h"
#include "Gameplay/Components/CameraComponent.h"
#include "Gameplay/EntityManager.h"

#include "Levels/LevelGenerator.h"

#include "Core/Settings/SettingsManager.h"

#include "SceneGraph/SceneGraph.h"
#include "ai/Path.h"
#include "ai/TrafficCar.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Levels/NavigationGraph.h"
#include "ai/AIComponent.h"
#include "ai/SteeringBehaviors.h"
#include <cstdint>
#include <sys/types.h>

LevelScene::LevelScene(const std::string& path) : levelPath(path) {}

void LevelScene::Load() {
    cityAmbientSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/music/city_ambient.mp3");
    cityAmbientSound->PlayBackgroundMusic(2.f);

    auto cameraEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("Camera", false, mlg::SceneGraph::GetRoot());
    auto cameraComponent = cameraEntity.lock()->AddComponent<mlg::CameraComponent>("CameraComponent");

    std::vector<std::string> levelLayout;

    levelLayout = mlg::LevelGenerator::LoadMap(levelPath);
    mlg::LevelGenerator::SpawnGround(levelPath);
    mlg::LevelGenerator::SetCityBounds(levelPath);
    mlg::LevelGenerator::LoadCameraSettings(levelPath, *cameraComponent.lock());
    mlg::LevelGenerator::SpawnPlayers(levelPath);

    navigationGraph = std::make_shared<NavigationGraph>(levelPath);

    // TODO: 
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

        auto aiComponent =
                aiCar.lock()->GetComponentByName<AIComponent>("AIComponent").lock();

        i++;

    }
}

void LevelScene::Update() {
#ifdef DEBUG
    if (!mlg::SettingsManager::Get<bool>(
                mlg::SettingsType::Debug, "showNavigation"))
        return;

    if (navigationGraph == nullptr)
        return;

    navigationGraph->DrawNodes();
#endif
}

const std::shared_ptr<NavigationGraph>& LevelScene::GetNavigationGraph() const {
    return navigationGraph;
}
