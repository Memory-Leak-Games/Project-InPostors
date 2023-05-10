#include <fstream>
#include <effolkronium/random.hpp>

#include "Rendering/Model.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "Gameplay/Levels/MapObject.h"
#include "Gameplay/Levels/LevelGenerator.h"

#include "Gameplay/EntityManager.h"
#include "Gameplay/Entity.h"

#include "SceneGraph/SceneGraph.h"

#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Physics/Colliders/ColliderShapes.h"

using json = nlohmann::json;
using Random = effolkronium::random_static;

namespace mlg {
    LevelGenerator* LevelGenerator::instance;

    void LevelGenerator::Initialize() {
        if (instance != nullptr) return;

        SPDLOG_INFO("Initializing Level Generator");
        instance = new LevelGenerator;
    }

    void LevelGenerator::Stop() {
        SPDLOG_INFO("Stopping Level Generator");
        delete instance;
        instance = nullptr;
    }

    LevelGenerator* LevelGenerator::GetInstance() {
        return instance;
    }

    void LevelGenerator::LoadJson(const std::string& path) {
        LoadLayout(path);
        LoadMapObjects(path);
    }

    void LevelGenerator::LoadLayout(const std::string& path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        if (!levelLayout.empty())
            levelLayout.clear();

        for (const auto& jsonLayoutString: levelJson["layout"]) {
            levelLayout.push_back(jsonLayoutString.get<std::string>());
        }

        tileSize = levelJson.contains("tile-size")
                   ? levelJson["tile-size"].get<float>() : 10.0f;
        defaultLevelMaterial = levelJson["default-material"].get<std::string>();
    }

    void LevelGenerator::LoadMapObjects(const std::string& path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        mapObjects = std::make_unique<std::unordered_map<char, LevelGenerator::MapEntry>>();

        for (const auto& jsonTile: levelJson["tiles"]) {
            const char tileSymbol = jsonTile["symbol"].get<std::string>()[0];
            std::vector<std::shared_ptr<MapObject>> mapObjectPool;

            for (const auto& jsonMapObject: jsonTile["objects"]) {
                mapObjectPool.push_back(ParseObject(jsonMapObject));
            }

            mapObjects->insert({tileSymbol, {std::move(mapObjectPool), 0}});
        }
    }

    std::shared_ptr<MapObject> LevelGenerator::ParseObject(nlohmann::json jsonMapObject) {
        std::string modelPath = jsonMapObject["model"].get<std::string>();
        std::string materialPath = jsonMapObject.contains("material")
                                   ? jsonMapObject["material"].get<std::string>()
                                   : defaultLevelMaterial;

        auto mapObj = std::make_shared<MapObject>();
        mapObj->model = AssetManager::GetAsset<ModelAsset>(modelPath);
        mapObj->material = AssetManager::GetAsset<MaterialAsset>(materialPath);

        mapObj->worldRot = jsonMapObject.contains("rotation")
                           ? jsonMapObject["rotation"].get<float>() : 0.f;
        mapObj->worldRot = glm::radians(mapObj->worldRot);

        mapObj->scale = jsonMapObject.contains("scale")
                        ? jsonMapObject["scale"].get<float>() : 1.f;

        if (jsonMapObject.contains("collision-type")) {
            mapObj->hasCollision = true;
            mapObj->colliderType = jsonMapObject["collision-type"].get<std::string>();
            mapObj->colliderSize = jsonMapObject.contains("collision-size")
                                   ? jsonMapObject["collision-size"].get<float>() : 1.0f;
            mapObj->colliderOffset = jsonMapObject.contains("collision-offset")
                                     ? jsonMapObject["collision-offset"].get<float>() : 0.0f;
        }

        return mapObj;
    }

    void LevelGenerator::GenerateLevel() {
        glm::vec2 citySize{0.f};
        citySize.y = static_cast<float>(levelLayout.size());

        for (const std::string& row: levelLayout) {
            citySize.x = std::max(citySize.x, static_cast<float>(row.size()));
        }

        citySize *= tileSize;

        int i = 0, j = 0;
        for (const std::string& row: levelLayout) {
            ++j;
            for (const char& character: row) {
                ++i;

                if (character == ' ')
                    continue;

                MLG_ASSERT_MSG(mapObjects->find(character) != mapObjects->end(),
                               "Unknown character in tile map");

                //mapObjPool is std::vector<std::shared_ptr<MapObject>>
                auto mapObjPool = mapObjects->at(character).object;
                int useCount = (mapObjects->at(character).useCount)++;

                if (useCount > mapObjPool.size() - 1) {
                    useCount = 0;
                    Random::shuffle(mapObjPool);
                }

                auto mapObj = mapObjPool[useCount];

                glm::vec3 objectPos{0.0f};
                objectPos.z = static_cast<float>(i) * tileSize - citySize.x * 0.5f;
                objectPos.y = 0.0f;//-0.5f;
                objectPos.x = -static_cast<float>(j - 1) * tileSize + citySize.y * 0.5f;
                PutObject(mapObj, objectPos);
            }
            i = 0;
        }
    }

    void LevelGenerator::PutObject(const std::shared_ptr<MapObject>& obj, glm::vec3 pos) {
        auto modelEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
                Hash("MapObject", pos.x, pos.z), true, mlg::SceneGraph::GetRoot());

        auto staticMesh = modelEntity.lock()->AddComponent<mlg::StaticMeshComponent>(
                "StaticMesh", obj->model, obj->material);

        staticMesh.lock()->GetTransform().SetScale(glm::vec3{obj->scale});

        modelEntity.lock()->GetTransform().SetPosition(pos);
        modelEntity.lock()->GetTransform().SetEulerRotation({0.f, obj->worldRot, 0.f});

        // add collider if needed
        if (!obj->hasCollision)
            return;

        auto rb = modelEntity.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        std::string colType = obj->colliderType;
        if (colType == "rectangle") {
            rb.lock()->AddCollider<mlg::ColliderShape::Rectangle>(
                    glm::vec2(glm::vec2(obj->colliderOffset)),
                    glm::vec2(obj->colliderSize));
        } else if (colType == "circle") {
            rb.lock()->AddCollider<mlg::ColliderShape::Circle>(
                    glm::vec2(glm::vec2(obj->colliderSize)),
                    obj->colliderOffset);
        }
        rb.lock()->SetRotation(obj->worldRot);
        rb.lock()->SetKinematic(true);
    }

    std::string LevelGenerator::Hash(const std::string& hashString, float posX, float posY) {
        std::size_t h1 = std::hash<std::string>{}(hashString);
        auto h2 = static_cast<size_t>(posX * posY + posX);
        std::stringstream ss;
        ss << (h1 ^ (h2 << 1));
        return ss.str();
    }
}