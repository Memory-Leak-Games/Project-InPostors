#include "Gameplay/Levels/LevelGenerator.h"
#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"
#include "Gameplay/EntityManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/Components/StaticMeshComponent.h"
#include "SceneGraph/SceneGraph.h"
#include "Macros.h"
#include "Rendering/Model.h"

namespace mlg {

    /**
     * Generate a hardcoded level for testing purposes
     * @return True on successful level generation, otherwise false
     */
    bool LevelGenerator::GenerateTestLevel() {
        auto tardisMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Tardis/tardis_material.json");
        auto tardisModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Tardis/tardis.obj");

        auto whiteMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/white_material.json");
        auto planeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/plane.obj");

        auto ground = mlg::EntityManager::SpawnEntity<mlg::Entity>("Ground", true, mlg::SceneGraph::GetRoot());
        ground.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", planeModel, whiteMaterial);
        ground.lock()->GetTransform().SetPosition({0.f, -5.f, 0.f});
        ground.lock()->GetTransform().SetScale(glm::vec3{100.f});

        for (unsigned int i = 0; i < 10; i++)
        {
            for (unsigned int k = 0; k < 10; k++)
            {
                if (k % 5 == 0) continue; // testing something
                PutObject("res/models/Tardis/tardis.obj", -50.0f + 10.0f * (float)i, -50.0f + 10.0f * (float)k);
            }
        }

        return true;
    }

    bool LevelGenerator::GenerateLevel() {
        SPDLOG_WARN("Not implemented yet!");
        return false;
    }

    bool LevelGenerator::PutObject(const std::string& modelPath, float posX, float posY, float rotation) {
        auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(modelPath);
        auto modelMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/white_material.json"); //don't @ me
        auto modelEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>(Hash(modelPath, posX, posY), false, mlg::SceneGraph::GetRoot());
        modelEntity.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", model, modelMaterial);
        modelEntity.lock()->GetTransform().SetPosition({posX, -5.f, posY});

        return true;
    }

    // will improve later, psycha is sitting
    std::string LevelGenerator::Hash(const std::string &modelPath, float posX, float posY) {
        std::size_t h1 = std::hash<std::string>{}(modelPath);
        auto h2 = static_cast<size_t>(posX * posY);
        std::stringstream ss;
        ss << (h1 ^ (h2 << 1));
        return ss.str();
    }
}