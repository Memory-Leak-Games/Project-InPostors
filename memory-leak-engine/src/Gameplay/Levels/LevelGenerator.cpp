#include <fstream>
#include <effolkronium/random.hpp>
#include <utility>

#include "Rendering/Model.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

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

    void LevelGenerator::LoadMap(const std::string& path) {
        LevelGenerator levelGenerator;

        std::ifstream levelFile{path};
        levelGenerator.levelJson = json::parse(levelFile);

        levelGenerator.LoadLayout();
        levelGenerator.LoadMapObjects();

        levelGenerator.GenerateLevel();
    }

    void LevelGenerator::LoadLayout() {
        for (const auto& jsonLayoutString: levelJson["layout"]) {
            levelLayout.push_back(jsonLayoutString.get<std::string>());
        }

        defaultMaterial = levelJson["defaultMaterial"].get<std::string>();
        tileSize = levelJson["tileSize"].get<float>();
    }

    void LevelGenerator::LoadMapObjects() {
        for (const auto& jsonTile: levelJson["tiles"]) {
            const char tileSymbol = jsonTile["symbol"].get<std::string>()[0];
            std::vector<MapObject> mapObjectPool;

            RoadType roadType;
            if (jsonTile.contains("roadType")) {
                const std::string roadTypeStr = jsonTile["roadType"].get<std::string>();
                roadType = magic_enum::enum_cast<RoadType>(roadTypeStr).value_or(RoadType::None);
            }

            for (const auto& jsonMapObject: jsonTile["objects"]) {
                mapObjectPool.push_back(std::move(ParseObject(jsonMapObject)));
            }

            mapObjects.insert({tileSymbol, MapEntry{roadType, mapObjectPool, 0}});
        }
    }

    LevelGenerator::MapObject LevelGenerator::ParseObject(const json& jsonMapObject) {
        std::string modelPath = jsonMapObject["model"].get<std::string>();
        std::string materialPath = jsonMapObject.contains("material")
                                   ? jsonMapObject["material"].get<std::string>()
                                   : defaultMaterial;

        MapObject mapObj;
        mapObj.model = AssetManager::GetAsset<ModelAsset>(modelPath);
        mapObj.material = AssetManager::GetAsset<MaterialAsset>(materialPath);

        mapObj.worldRot = jsonMapObject.contains("rotation") ? jsonMapObject["rotation"].get<float>() : 0.f;
        mapObj.worldRot = glm::radians(mapObj.worldRot);

        mapObj.scale = jsonMapObject.contains("scale")
                        ? jsonMapObject["scale"].get<float>() : 1.f;

        if (jsonMapObject.contains("isCorner"))
            mapObj.isCorner = jsonMapObject["isCorner"].get<bool>();

        if (jsonMapObject.contains("collision-type")) {
            mapObj.hasCollision = true;
            mapObj.colliderType = jsonMapObject["collision-type"].get<std::string>();
            mapObj.colliderSize = jsonMapObject.contains("collision-size")
                                   ? jsonMapObject["collision-size"].get<float>() : 1.0f;
            mapObj.colliderOffset = jsonMapObject.contains("collision-offset")
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

                MLG_ASSERT_MSG(mapObjects.find(character) != mapObjects.end(),
                               "Unknown character in tile map");

                MapEntry& mapEntry = mapObjects.at(character);
                std::vector<MapObject>& mapObjectPool = mapEntry.objectsPool;

                mapEntry.useCount++;

                if (mapEntry.useCount > mapObjectPool.size() - 1) {
                    mapEntry.useCount = 0;
                    Random::shuffle(mapObjectPool);
                }

                glm::vec3 objectPosition{0.0f};
                objectPosition.z = static_cast<float>(i) * tileSize - citySize.x * 0.5f;
                objectPosition.x = -static_cast<float>(j - 1) * tileSize + citySize.y * 0.5f;

                float smartRotation = 0;
                if (mapEntry.roadType == RoadType::None)
                    smartRotation = GetSmartRotation({j - 1, i - 1});

                PutObject(mapObjectPool[mapEntry.useCount], objectPosition, smartRotation);
            }
            i = 0;
        }
    }

    void LevelGenerator::PutObject(const MapObject& mapObject, const glm::vec3& position, float rotation) {
        auto modelEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("MapObject", true, mlg::SceneGraph::GetRoot());

        auto staticMesh = modelEntity.lock()->
                AddComponent<mlg::StaticMeshComponent>("StaticMesh", mapObject.model, mapObject.material);

        modelEntity.lock()->GetTransform().SetPosition(position);
        modelEntity.lock()->GetTransform().SetEulerRotation({0.f, mapObject.worldRot + rotation, 0.f});
        modelEntity.lock()->GetTransform().SetScale(glm::vec3{mapObject.scale});

        if (!mapObject.hasCollision)
            return;

        // add collider if needed
        auto rb = modelEntity.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        std::string colType = mapObject.colliderType;
        if (colType == "rectangle") {
            rb.lock()->AddCollider<mlg::ColliderShape::Rectangle>(
                    glm::vec2(glm::vec2(mapObject.colliderOffset)),
                    glm::vec2(mapObject.colliderSize));
        } else if (colType == "circle") {
            rb.lock()->AddCollider<mlg::ColliderShape::Circle>(
                    glm::vec2(glm::vec2(mapObject.colliderSize)),
                    mapObject.colliderOffset);
        }

        rb.lock()->SetRotation(mapObject.worldRot + rotation);
        rb.lock()->SetKinematic(true);
    }

    float LevelGenerator::GetSmartRotation(const glm::ivec2& layoutPosition) {
        const std::vector<glm::ivec2> directions = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

        float angle = 0;
        for (auto direction : directions) {
            angle += glm::radians(90.f);

            const glm::ivec2 neighbourPosition = layoutPosition + direction;

            if (neighbourPosition.x < 0 || neighbourPosition.y < 0)
                continue;

            if (levelLayout.size() - 1 < neighbourPosition.x)
                continue;

            if (levelLayout[neighbourPosition.x].size() - 1 < neighbourPosition.y)
                continue;

            const char neighbour = levelLayout[neighbourPosition.x][neighbourPosition.y];

            if (neighbour == ' ')
                continue;

            MapEntry& neighbourEntry = mapObjects[neighbour];

            if (neighbourEntry.roadType != RoadType::None)
                return angle - glm::radians(90.f);

        }

        return angle;
    }

}