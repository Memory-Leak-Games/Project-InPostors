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
    float LevelGenerator::tileSize;
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
        levelWidth = levelJson["max-width"].get<int>();
        levelHeight = levelJson["max-height"].get<int>();
        tileSize = levelJson.contains("tile-size") ? levelJson["tile-size"].get<float>() : 10.0f;

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


    void LevelGenerator::GenerateLevel() {
        float citySize = tileSize * static_cast<float>(levelLayout.size());
        for (unsigned int i = 0; i < levelHeight; i++)
        {
            if (i > levelLayout.size() - 1) // We have reached end of this layout
                break;

            for (unsigned int k = 0; k < levelWidth; k++)
            {
                if (k > levelLayout[i].size() - 1) // We have reached end of this layout line
                    break;

                std::string symbol(1, levelLayout[i][k]);
                if (symbol == " ") // We do not put any map object here
                    continue;

                auto mapObjPool = mapObjects->at(symbol);
                // Get random MapObject from pool
                int rand = 0;
                int poolSize = (int)mapObjPool.size();
                if (poolSize > 1) // If there is only one object in the pool, skip random pick
                    rand = Random::get(0, poolSize - 1);

                auto mapObj = mapObjPool[rand];
                glm::vec3 objectPos{0.0f};
                objectPos.x = static_cast<float>(k) * tileSize - citySize * 0.5f;
                objectPos.y = -0.5f;
                objectPos.z = static_cast<float>(i) * tileSize - citySize * 0.5f;
                PutObject(mapObj, objectPos);
            }
        }
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