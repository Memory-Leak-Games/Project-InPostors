#include "Gameplay/Levels/LevelGenerator.h"
#include "Core/AssetManager/AssetManager.h"
#include "Gameplay/EntityManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/Components/StaticMeshComponent.h"
#include "SceneGraph/SceneGraph.h"
#include "Macros.h"
#include "Rendering/Model.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Physics/Colliders/ColliderShapes.h"

namespace mlg {
    std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<MapObject>>>
    LevelGenerator::LoadJson(const std::string &path) {
        auto tileMap = std::make_unique<
                std::unordered_map<std::string, std::unique_ptr<MapObject>>>();

        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        for (const auto& jsonTile : levelJson["tiles"]) {
            std::string tileSymbol = jsonTile["symbol"].get<std::string>();
            std::string modelPath = jsonTile["model"].get<std::string>();
            std::string materialPath = jsonTile["material"].get<std::string>();
            float yRot = jsonTile.contains("rotation") ? jsonTile["rotation"].get<float>() : 0.0f;

            mlg::MapObject mapObj(modelPath, materialPath, yRot);
            if (jsonTile.contains("collision-type")) {
                std::string cType = jsonTile["collision-type"].get<std::string>();
                float cSize = jsonTile.contains("collision-size") ? jsonTile["collision-size"].get<float>() : 1.0f;
                float cOffset = jsonTile.contains("collision-offset") ? jsonTile["collision-offset"].get<float>(): 0.0f;
                mapObj.AddCollision(cType, cSize, cOffset);
            }

            auto newMapObj = std::make_unique<MapObject>(mapObj);
            tileMap->insert({tileSymbol, std::move(newMapObj)});
        }

        return std::move(tileMap);
    }


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

        MapObject testObject("res/models/Tardis/tardis.obj", "res/models/Tardis/tardis_material.json");
        auto mObj = std::make_shared<MapObject>(testObject);

        for (unsigned int i = 0; i < 9; i++)
        {
            for (unsigned int k = 0; k < 9; k++)
            {
                if (k % 5 == 0) continue; // testing something
                //PutObject("res/models/Tardis/tardis.obj", "res/models/Tardis/tardis_material.json",
                //PutObject(tardisModel, tardisMaterial,
                //        {-40.0f + 10.0f * (float)i, 0.0f, -40.0f + 10.0f * (float)k},
                //        90.0f * (float)(i % 4));
                PutObject(mObj, {-40.0f + 10.0f * (float)i, 0.0f, -40.0f + 10.0f * (float)k});
            }
        }

        return true;
    }


    bool LevelGenerator::GenerateLevel(const mlg::LevelLayout& layout) {
        SPDLOG_WARN("Not implemented yet!");
        return false;
    }


    void LevelGenerator::PutObject(const std::string& modelPath, const std::string& materialPath, glm::vec3 pos, float rotation) {
        auto modelAsset = mlg::AssetManager::GetAsset<mlg::ModelAsset>(modelPath);
        auto modelMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(materialPath);
        PutObject(modelAsset, modelMaterial, pos, rotation);
    }


    void LevelGenerator::PutObject(std::shared_ptr<ModelAsset>& modelAsset, std::shared_ptr<MaterialAsset>& materialAsset, glm::vec3 pos, float rotation) {
        auto modelEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>(Hash("MapObject", pos.x, pos.z), true, mlg::SceneGraph::GetRoot());
        modelEntity.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", modelAsset, materialAsset);
        modelEntity.lock()->GetTransform().SetPosition(pos);
        modelEntity.lock()->GetTransform().SetEulerRotation({0.0f, glm::radians(rotation), 0.0f});

    }


    void LevelGenerator::PutObject(const std::shared_ptr<MapObject>& obj, glm::vec3 pos) {
        auto modelEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
                Hash("MapObject", pos.x, pos.z),
                true, mlg::SceneGraph::GetRoot());
        modelEntity.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", obj->GetModel().lock(), obj->GetMaterial().lock());
        modelEntity.lock()->GetTransform().SetPosition(pos);
        modelEntity.lock()->GetTransform().SetEulerRotation(obj->GetRotation());

        // add collider if needed
        if (!obj->HasCollision())
            return;
        auto rigidbody = modelEntity.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        std::string colType = obj->GetColliderType();
        if (colType == "rectangle") {
            rigidbody.lock()->AddCollider<mlg::ColliderShape::Rectangle>(
                    glm::vec2(glm::vec2(obj->GetColliderSize())), glm::vec2(obj->GetColliderOffset()));
        }
        else if (colType == "circle") {
            rigidbody.lock()->AddCollider<mlg::ColliderShape::Circle>(
                    glm::vec2(glm::vec2(obj->GetColliderSize())), obj->GetColliderOffset());
        }
    }

    std::string LevelGenerator::Hash(const std::string &hashString, float posX, float posY) {
        std::size_t h1 = std::hash<std::string>{}(hashString);
        auto h2 = static_cast<size_t>(posX * posY + posX);
        std::stringstream ss;
        ss << (h1 ^ (h2 << 1));
        return ss.str();
    }

}