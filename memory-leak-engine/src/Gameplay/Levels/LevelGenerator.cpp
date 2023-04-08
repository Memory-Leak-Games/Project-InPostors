#include "Gameplay/Levels/LevelGenerator.h"
#include "Core/AssetManager/AssetManager.h"
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

        for (unsigned int i = 0; i < 9; i++)
        {
            for (unsigned int k = 0; k < 9; k++)
            {
                if (k % 5 == 0) continue; // testing something
                //PutObject("res/models/Tardis/tardis.obj", "res/models/Tardis/tardis_material.json",
                PutObject(tardisModel, tardisMaterial,
                          {-40.0f + 10.0f * (float)i, 0.0f, -40.0f + 10.0f * (float)k},
                          90 * i % 4);
            }
        }

        return true;
    }

    bool LevelGenerator::GenerateLevel() {
        SPDLOG_WARN("Not implemented yet!");
        return false;
    }

    bool LevelGenerator::PutObject(const std::string& modelPath, const std::string& materialPath, glm::vec3 pos, float rotation) {
        auto modelAsset = mlg::AssetManager::GetAsset<mlg::ModelAsset>(modelPath);
        auto modelMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(materialPath);
        return PutObject(modelAsset, modelMaterial, pos, rotation);
    }

    bool LevelGenerator::PutObject(std::shared_ptr<ModelAsset>& modelAsset, std::shared_ptr<MaterialAsset>& materialAsset, glm::vec3 pos, float rotation) {
        auto modelEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>(Hash("MapObject", pos.x, pos.z), false, mlg::SceneGraph::GetRoot());
        modelEntity.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", modelAsset, materialAsset);
        modelEntity.lock()->GetTransform().SetPosition(pos);
        modelEntity.lock()->GetTransform().SetEulerRotation({0.0f, glm::radians(rotation), 0.0f});

        return true;
    }

    std::string LevelGenerator::Hash(const std::string &modelPath, float posX, float posY) {
        std::size_t h1 = std::hash<std::string>{}(modelPath);
        auto h2 = static_cast<size_t>(posX * posY + posX);
        std::stringstream ss;
        ss << (h1 ^ (h2 << 1));
        return ss.str();
    }
}