#include "Scenes/LevelScene.h"

#include "Gameplay/EntityManager.h"
#include "Gameplay/Components/CameraComponent.h"

#include "Levels/LevelGenerator.h"

#include "SceneGraph/SceneGraph.h"

LevelScene::LevelScene(const std::string& path) : levelPath(path) {}

void LevelScene::Load() {
    auto cameraEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("Camera", false, mlg::SceneGraph::GetRoot());
    auto cameraComponent = cameraEntity.lock()->AddComponent<mlg::CameraComponent>("CameraComponent");

    std::vector<std::string> levelLayout;

    levelLayout = mlg::LevelGenerator::LoadMap(levelPath);
    mlg::LevelGenerator::SpawnGround(levelPath);
    mlg::LevelGenerator::SetCityBounds(levelPath);
    mlg::LevelGenerator::LoadCameraSettings(levelPath, *cameraComponent.lock());
    mlg::LevelGenerator::SpawnPlayers(levelPath);
}

