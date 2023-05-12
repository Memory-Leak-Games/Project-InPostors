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

#include "Gameplay/Components/CameraComponent.h"

#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Physics/Colliders/ColliderShapes.h"

using json = nlohmann::json;
using Random = effolkronium::random_static;

namespace mlg {

    void LevelGenerator::LoadCameraSettings(const std::string& path, struct CameraComponent& cameraComponent) {
        std::ifstream levelFile{path};
        json cameraJson = json::parse(levelFile)["cameraSettings"];

        const glm::vec3 position = {
                cameraJson["position"][0].get<float>(),
                cameraJson["position"][1].get<float>(),
                cameraJson["position"][2].get<float>()
        };

        const glm::vec3 eulerRotation = {
                cameraJson["rotation"][0].get<float>(),
                cameraJson["rotation"][1].get<float>(),
                cameraJson["rotation"][2].get<float>()
        };

        const float size = cameraJson["size"].get<float>();
        const float near = cameraJson["near"].get<float>();
        const float far = cameraJson["far"].get<float>();

        cameraComponent.GetTransform().SetPosition(position);
        cameraComponent.GetTransform().SetRotation(glm::radians(eulerRotation));
        cameraComponent.SetOrtho(size, near, far);
    }

    void LevelGenerator::SpawnGround(const std::string& path) {
        LevelGenerator levelGenerator;
        std::ifstream levelFile{path};

        levelGenerator.levelJson = json::parse(levelFile);
        levelGenerator.LoadLayout();

        const glm::vec2 citySize = levelGenerator.GetCitySize();

        glm::vec4 color {
            levelGenerator.levelJson["groundColor"][0].get<float>(),
            levelGenerator.levelJson["groundColor"][1].get<float>(),
            levelGenerator.levelJson["groundColor"][2].get<float>(),
            levelGenerator.levelJson["groundColor"][3].get<float>()
        };

        auto planeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/plane.obj");
        auto groundMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Ground/ground_material.json");
        groundMaterial = groundMaterial->CreateDynamicInstance();
        groundMaterial->SetVec4("color", color);

        auto ground = mlg::EntityManager::SpawnEntity<mlg::Entity>("Ground", true, mlg::SceneGraph::GetRoot());
        ground.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", planeModel, groundMaterial);
        ground.lock()->GetTransform().SetPosition({0.f, -0.01f, 0.f});
        ground.lock()->GetTransform().SetScale(glm::vec3{100.f});
    }

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
        citySize = GetCitySize();
    }

    void LevelGenerator::LoadMapObjects() {
        std::ifstream tileFile{levelJson["tileset"].get<std::string>()};
        tileJson = json::parse(tileFile);

        for (const auto& jsonTile: tileJson["tiles"]) {
            const char tileSymbol = jsonTile["symbol"].get<std::string>()[0];

            if (tileSymbol == roadsObjects.symbol)
                continue;

            std::vector<MapObject> mapObjectPool;

            bool isPathWay = false;
            if (jsonTile.contains("isPathWay")) {
                isPathWay = jsonTile["isPathWay"].get<bool>();
            }

            for (const auto& jsonMapObject: jsonTile["objects"]) {
                mapObjectPool.push_back(std::move(ParseObject(jsonMapObject)));
            }

            mapObjects.insert({tileSymbol, MapEntry{mapObjectPool, isPathWay, 0}});
        }
        LoadRoads();
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

    void LevelGenerator::LoadRoads() {
        json roadJson = tileJson["road"];
        roadsObjects.symbol = roadJson["symbol"].get<std::string>()[0];

        roadsObjects.road = ParseObject(roadJson["road"]);
        roadsObjects.edge = ParseObject(roadJson["edge"]);
        roadsObjects.cross = ParseObject(roadJson["cross"]);
        roadsObjects.curve = ParseObject(roadJson["curve"]);
    }

    void LevelGenerator::GenerateLevel() {

        int i = 0, j = 0;
        for (const std::string& row: levelLayout) {
            ++j;
            for (const char& character: row) {
                ++i;

                if (character == ' ')
                    continue;

                if (character == roadsObjects.symbol) {
                    PutRoad(j - 1, i - 1);
                    continue;
                }

                PutTile(j - 1, i - 1, character);
            }
            i = 0;
        }
    }

    glm::vec2 LevelGenerator::GetCitySize() {
        glm::vec2 citySize;
        for (const std::string& row: levelLayout) {
            citySize.x = std::max(citySize.x, static_cast<float>(row.size()));
        }
        citySize.y = (float) levelLayout.size();

        citySize *= tileSize;
        return citySize;
    }

    void LevelGenerator::PutTile(int x, int y, const char& character) {
        if (!mapObjects.contains(character)) {
            SPDLOG_WARN("LevelLoader: unknown character: {}, at ({}, {}).", character, x, y);
            return;
        }

        MapEntry& mapEntry = mapObjects.at(character);
        std::vector<MapObject>& mapObjectPool = mapEntry.objectsPool;

        mapEntry.useCount++;

        if (mapEntry.useCount > mapObjectPool.size() - 1) {
            mapEntry.useCount = 0;
            Random::shuffle(mapObjectPool);
        }

        float smartRotation = GetSmartRotation({x, y});
        PutEntity(mapObjectPool[mapEntry.useCount], glm::ivec2{x, y}, smartRotation);
    }

    void LevelGenerator::PutEntity(const MapObject& mapObject, const glm::ivec2& position, float rotation) {
        auto modelEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("MapObject", true, mlg::SceneGraph::GetRoot());

        auto staticMesh = modelEntity.lock()->
                AddComponent<mlg::StaticMeshComponent>("StaticMesh", mapObject.model, mapObject.material);

        glm::vec3 objectPosition{0.0f};
        objectPosition.x = -static_cast<float>(position.x) * tileSize + citySize.y * 0.5f;
        objectPosition.z = static_cast<float>(position.y + 1) * tileSize - citySize.x * 0.5f;

        modelEntity.lock()->GetTransform().SetPosition(objectPosition);
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

            if (neighbour == roadsObjects.symbol)
                continue;

            MapEntry& neighbourEntry = mapObjects.at(neighbour);

            if (neighbourEntry.isPathWay)
                return angle - glm::radians(90.f);
        }

        return angle;
    }

    void LevelGenerator::PutRoad(int x, int y) {
        Neighbours neighbours = GetNeighbours(x, y);
        int neighboursRoadsCount = 0;
        neighboursRoadsCount += neighbours.down == roadsObjects.symbol ? 1 : 0;
        neighboursRoadsCount += neighbours.left == roadsObjects.symbol ? 1 : 0;
        neighboursRoadsCount += neighbours.right == roadsObjects.symbol ? 1 : 0;
        neighboursRoadsCount += neighbours.up == roadsObjects.symbol ? 1 : 0;

        bool isEdge = neighboursRoadsCount == 3;
        bool isCross = neighboursRoadsCount == 4;

        bool isVertical = neighbours.up == roadsObjects.symbol || neighbours.down == roadsObjects.symbol;
        isVertical &= neighbours.left != roadsObjects.symbol && neighbours.right != roadsObjects.symbol;

        bool isHorizontal = neighbours.left == roadsObjects.symbol || neighbours.right == roadsObjects.symbol;
        isHorizontal &= neighbours.up != roadsObjects.symbol && neighbours.down != roadsObjects.symbol;

        if (isEdge) {
            PutEdgeRoad(x, y);
        } else if (isCross) {
            PutCrossRoad(x, y);
        } else if (isVertical) {
            PutStraightRoad(x, y, false);
        } else if (isHorizontal) {
            PutStraightRoad(x, y, true);
        } else {
//            PutCurveRoad(x, y);
        }

    }

    void LevelGenerator::PutCrossRoad(int x, int y) {
        auto modelEntity = EntityManager::SpawnEntity<mlg::Entity>("MapObject", true, mlg::SceneGraph::GetRoot());
        auto mapObject = roadsObjects.cross;

        auto staticMesh = modelEntity.lock()->
                AddComponent<mlg::StaticMeshComponent>("StaticMesh", mapObject.model, mapObject.material);

        glm::vec3 objectPosition{0.0f};
        objectPosition.z = static_cast<float>(y + 1) * tileSize - citySize.x * 0.5f;
        objectPosition.x = -static_cast<float>(x) * tileSize + citySize.y * 0.5f;

        modelEntity.lock()->GetTransform().SetPosition(objectPosition);
    }

    void LevelGenerator::PutStraightRoad(int x, int y, bool isHorizontal) {
        float angle = isHorizontal ? 90.f : 0.f;
        auto mapObject = roadsObjects.road;

        PutEntity(roadsObjects.road, glm::ivec2{x, y}, glm::radians(angle));
    }

    void LevelGenerator::PutEdgeRoad(int x, int y) {
        float angle;
        Neighbours neighbours = GetNeighbours(x, y);

        if (neighbours.left != roadsObjects.symbol)
            angle = 0.f;
        else if (neighbours.down != roadsObjects.symbol)
            angle = 90.f;
        else if (neighbours.right != roadsObjects.symbol)
            angle = 180.f;
        else
            angle = 270.f;

        PutEntity(roadsObjects.edge, glm::ivec2{x, y}, glm::radians(angle));
    }

    LevelGenerator::Neighbours LevelGenerator::GetNeighbours(int x, int y) {
        const std::vector<glm::ivec2> directions = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        Neighbours result;
        char* neighbour = (char*) &result - 1;

        for (auto direction : directions) {
            const glm::ivec2 neighbourPosition = glm::ivec2(x,y) + direction;
            neighbour++;

            if (neighbourPosition.x < 0 || neighbourPosition.y < 0)
                continue;

            if (levelLayout.size() - 1 < neighbourPosition.x)
                continue;

            if (levelLayout[neighbourPosition.x].size() - 1 < neighbourPosition.y)
                continue;

            *neighbour = levelLayout[neighbourPosition.x][neighbourPosition.y];
        }

        return result;
    }

}