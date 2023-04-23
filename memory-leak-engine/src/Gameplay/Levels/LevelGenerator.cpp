#include "Gameplay/Levels/LevelGenerator.h"
#include "Core/AssetManager/AssetManager.h"
#include "Gameplay/EntityManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/Components/StaticMeshComponent.h"
#include "SceneGraph/SceneGraph.h"
#include "Macros.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Physics/Colliders/ColliderShapes.h"
#include <fstream>

namespace mlg {
    LevelGenerator* LevelGenerator::instance;
    int LevelGenerator::levelWidth;
    int LevelGenerator::levelHeight;
    std::vector<std::vector<char>> LevelGenerator::levelLayout;
    std::unique_ptr<std::unordered_map<std::string,
            std::vector<std::shared_ptr<MapObject>>>> LevelGenerator::mapObjects;

    void LevelGenerator::Initialize() {
        if (instance != nullptr) return;

        SPDLOG_INFO("Initializing LevelGenerator");
        instance = new LevelGenerator;
    }

    void LevelGenerator::Stop() {
        SPDLOG_INFO("Stopping LevelGenerator");
        delete instance;
        instance = nullptr;
    }

    LevelGenerator* LevelGenerator::GetInstance() {
        return LevelGenerator::instance;
    }

    void LevelGenerator::LoadJson(const std::string &path) {

        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        for (const auto& jsonLayoutString : levelJson["layout"]) {
            std::vector<char> layoutString;
            std::string buf = jsonLayoutString;
            for (char i : buf) {
                layoutString.push_back(i);
            }
            levelLayout.push_back(layoutString);
        }
        levelWidth = 5; //levelJson["width"].get<int>();
        levelHeight = 5; // levelJson["height"].get<int>();

        mapObjects = std::make_unique<std::unordered_map<std::string,
                std::vector<std::shared_ptr<MapObject>>>>();

        for (const auto& jsonTile : levelJson["tiles"]) {
            std::string tileSymbol = jsonTile["symbol"].get<std::string>();
            std::vector<std::shared_ptr<MapObject>> mapObjectPool;

            for (const auto& jsonMapObject : jsonTile["objects"])
            {
                std::string modelPath = jsonMapObject["model"].get<std::string>();
                std::string materialPath = jsonMapObject["material"].get<std::string>();
                float yRot = jsonMapObject.contains("rotation") ? jsonMapObject["rotation"].get<float>() : 0.0f;

                mlg::MapObject mapObj(modelPath, materialPath, yRot);
                if (jsonMapObject.contains("collision-type")) {
                    std::string cType = jsonMapObject["collision-type"].get<std::string>();
                    float cSize = jsonMapObject.contains("collision-size") ? jsonMapObject["collision-size"].get<float>() : 1.0f;
                    float cOffset = jsonMapObject.contains("collision-offset") ? jsonMapObject["collision-offset"].get<float>(): 0.0f;
                    mapObj.AddCollision(cType, cSize, cOffset);
                }
                //float hOffset = jsonTile.contains("position-offset-h") ? jsonTile["position-offset-h"].get<float>() : 0.0f;
                //float vOffset = jsonTile.contains("position-offset-v") ? jsonTile["position-offset-v"].get<float>() : 0.0f;
                //mapObj.SetPositionOffset(hOffset, vOffset);

                auto newMapObj = std::make_shared<MapObject>(mapObj);
                mapObjectPool.push_back(newMapObj);
            }
            mapObjects->insert({tileSymbol, std::move(mapObjectPool)});
        }
    }


    void LevelGenerator::GenerateLevel(float tileSize) {
        float citySize = tileSize * static_cast<float>(levelLayout.size());
        for (unsigned int i = 0; i < levelWidth; i++)
        {
            for (unsigned int k = 0; k < levelHeight; k++)
            {
                try {
                    std::string symbol(1, levelLayout[i][k]);
                    if (symbol == " ")
                        continue;
                    auto mapObjPool = mapObjects->at(symbol);
                    // Get random MapObject from pool
                    int rand = 0;
                    int poolSize = (int)mapObjPool.size();
                    if (poolSize > 1) // If there is only one object in the pool, skip random pick
                        rand = Random::get(0, poolSize - 1);

                    auto mapObj = mapObjPool[rand];
                    glm::vec3 objectPos{0.0f};
                    objectPos.x = static_cast<float>(i) * tileSize - citySize * 0.5f;
                    objectPos.y = -0.5f;
                    objectPos.z = static_cast<float>(k) * tileSize - citySize * 0.5f;
                    PutObject(mapObj, objectPos);
                }
                catch (const std::out_of_range &e) {
                    SPDLOG_CRITICAL("Exception at {}", e.what());
                }
            }
        }
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
                    glm::vec2(glm::vec2(obj->GetColliderOffset())), glm::vec2(obj->GetColliderSize()));
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