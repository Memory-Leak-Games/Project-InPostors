#include <effolkronium/random.hpp>
#include <fstream>
#include <utility>

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Model.h"

#include "Levels/LevelGenerator.h"

#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"

#include "SceneGraph/SceneGraph.h"

#include "Gameplay/Components/CameraComponent.h"

#include "Car/PlayerOneInput.h"
#include "Car/PlayerTwoInput.h"
#include "Core/RGBA.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"
#include "Physics/Colliders/ColliderShapes.h"
#include "Physics/CollisionManager.h"
#include "Player.h"

#include "Buildings/AutoDestroyComponent.h"

using json = nlohmann::json;
using Random = effolkronium::random_static;

namespace mlg {

    std::vector<std::string> LevelGenerator::LoadMap(const std::string& path) {
        LevelGenerator levelGenerator;

        std::ifstream levelFile{path};
        levelGenerator.levelJson = json::parse(levelFile);

        std::vector<std::string> ret = levelGenerator.LoadLayout();
        levelGenerator.LoadMapObjects();
        // I need to test full factories not only mesh
        // levelGenerator.LoadFactories();
        levelGenerator.LoadRoads();

        levelGenerator.GenerateLevel();

        return ret;
    }

    void LevelGenerator::LoadCameraSettings(const std::string& path, struct CameraComponent& cameraComponent) {
        std::ifstream levelFile{path};
        json cameraJson = json::parse(levelFile)["camera-settings"];

        const glm::vec3 position = {
                cameraJson["position"][0].get<float>(),
                cameraJson["position"][1].get<float>(),
                cameraJson["position"][2].get<float>()};

        const glm::vec3 eulerRotation = {
                cameraJson["rotation"][0].get<float>(),
                cameraJson["rotation"][1].get<float>(),
                cameraJson["rotation"][2].get<float>()};

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
        const float tileSize = levelGenerator.tileSize;

        glm::vec4 color{
                levelGenerator.levelJson["ground-color"][0].get<float>(),
                levelGenerator.levelJson["ground-color"][1].get<float>(),
                levelGenerator.levelJson["ground-color"][2].get<float>(),
                levelGenerator.levelJson["ground-color"][3].get<float>()};

        auto planeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/primitives/plane.obj");
        auto groundMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/color/gray_material.json");
        groundMaterial = groundMaterial->CreateDynamicInstance();
        groundMaterial->SetVec4("color", color);

        auto ground = mlg::EntityManager::SpawnEntity<mlg::Entity>("Ground", true, mlg::SceneGraph::GetRoot());
        ground.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", planeModel, groundMaterial);
        ground.lock()->GetTransform().SetPosition({0.f, -0.01f, 0.f});

        glm::vec3 groundScale{1.f};
        groundScale.x = citySize.x + 2 * tileSize;
        groundScale.z = citySize.y + 2 * tileSize;

        SPDLOG_DEBUG("Spawning Ground with size: ({}, {})", groundScale.x, groundScale.z);
        SPDLOG_DEBUG("City size: ({}, {})", citySize.x, citySize.y);
        SPDLOG_DEBUG("Tile size: {}", tileSize);

        ground.lock()->GetTransform().SetScale(groundScale);
    }

    void LevelGenerator::SetCityBounds(const std::string& path) {
        LevelGenerator levelGenerator;
        std::ifstream levelFile{path};

        levelGenerator.levelJson = json::parse(levelFile);
        levelGenerator.LoadLayout();

        const glm::vec2 citySize = levelGenerator.GetCitySize();
        const glm::ivec2 layoutSize = levelGenerator.GetLayoutSize();

        const glm::vec2 cityStart = -0.5f * citySize;
        const glm::vec2 cityEnd = 0.5f * citySize;

        CollisionManager::SetBounds(cityStart, cityEnd, layoutSize);
    }

    // ======== PRIVATE METHODS ========

    std::vector<std::string> LevelGenerator::LoadLayout() {
        std::vector<std::string> ret;
        for (const auto& jsonLayoutString : levelJson["layout"]) {
            std::string layoutString = jsonLayoutString.get<std::string>();
            levelLayout.push_back(layoutString);
            ret.push_back(layoutString);
        }

        tileSize = levelJson["tile-size"].get<float>();
        citySize = GetCitySize();
        return ret;
    }

    void LevelGenerator::LoadMapObjects() {
        std::ifstream tileFile{levelJson["tileset"].get<std::string>()};
        tileJson = json::parse(tileFile);

        defaultMaterial = tileJson["default-material"].get<std::string>();

        for (const auto& jsonTile : tileJson["tiles"]) {
            const char tileSymbol = jsonTile["symbol"].get<std::string>()[0];

            if (tileSymbol == roadsObjects.symbol)
                continue;

            std::vector<MapObject> mapObjectPool;

            bool isPathWay = false;
            if (jsonTile.contains("is-path-way")) {
                isPathWay = jsonTile["is-path-way"].get<bool>();
            }

            for (const auto& jsonMapObject : jsonTile["objects"]) {
                mapObjectPool.push_back(std::move(ParseObject(jsonMapObject)));
            }

            mapObjects.insert({tileSymbol, MapEntry{mapObjectPool, isPathWay, 0}});
        }

        if (levelJson.contains("ignore"))
            ignoredCharacters = levelJson["ignore"].get<std::string>();
    }

    void LevelGenerator::LoadFactories() {
        if (!tileJson.contains("factories"))
            return;

        for (const auto& jsonFactories : tileJson["factories"]) {
            const char tileSymbol = jsonFactories["symbol"].get<std::string>()[0];
            std::string configPath = jsonFactories["config"].get<std::string>();
            FactoryObject factoryTile = LoadFactoryData(configPath);
            std::vector<MapObject> mapObjectPool;//ugly.
            mapObjectPool.push_back(factoryTile.mesh);
            mapObjects.insert({tileSymbol, MapEntry{mapObjectPool, false, 0}});
        }
    }

    void LevelGenerator::LoadRoads() {
        if (!tileJson.contains("road"))
            return;

        json roadJson = tileJson["road"];
        roadsObjects.symbol = roadJson["symbol"].get<std::string>()[0];

        roadsObjects.road = ParseObject(roadJson["road"]);
        roadsObjects.edge = ParseObject(roadJson["edge"]);
        roadsObjects.cross = ParseObject(roadJson["cross"]);
        roadsObjects.curve = ParseObject(roadJson["curve"]);
        roadsObjects.corner = ParseObject(roadJson["corner"]);
    }

    LevelGenerator::MapObject LevelGenerator::ParseObject(const json& jsonMapObject) {
        std::string modelPath = jsonMapObject["model"].get<std::string>();
        std::string materialPath = jsonMapObject.contains("material")
                                           ? jsonMapObject["material"].get<std::string>()
                                           : defaultMaterial;

        MapObject mapObj;
        mapObj.model = AssetManager::GetAsset<ModelAsset>(modelPath);
        mapObj.material = AssetManager::GetAsset<MaterialAsset>(materialPath);

        mapObj.worldRot = jsonMapObject.contains("rotation")
                                  ? jsonMapObject["rotation"].get<float>()
                                  : 0.f;
        mapObj.worldRot = glm::radians(mapObj.worldRot);

        mapObj.scale = jsonMapObject.contains("scale")
                               ? jsonMapObject["scale"].get<float>()
                               : 1.f;

        if (jsonMapObject.contains("collision-type")) {
            mapObj.hasCollision = true;
            mapObj.colliderType = jsonMapObject["collision-type"].get<std::string>();
            mapObj.colliderSize = jsonMapObject.contains("collision-size")
                                          ? jsonMapObject["collision-size"].get<float>()
                                          : 1.0f;
            mapObj.colliderOffset = jsonMapObject.contains("collision-offset")
                                            ? jsonMapObject["collision-offset"].get<float>()
                                            : 0.0f;

            if (jsonMapObject.contains("dynamic"))
                mapObj.isDynamic = jsonMapObject["dynamic"];
        }

        return mapObj;
    }

    LevelGenerator::FactoryObject LevelGenerator::LoadFactoryData(const std::string& path) {
        std::ifstream factoryFile{path};
        json factoryJson = json::parse(factoryFile);

        FactoryObject factoryObj;
        factoryObj.mesh = ParseObject(factoryJson["static-mesh"]);
        //todo: add colliders, blueprints & emitters

        return factoryObj;
    }

    void LevelGenerator::GenerateLevel() {
        int x = 0, y = 0;
        for (const std::string& row : levelLayout) {
            ++y;
            for (const char& character : row) {
                ++x;

                // Ignore spaces
                if (character == ' ')
                    continue;

                // Ignore specified characters
                if (std::find(ignoredCharacters.begin(), ignoredCharacters.end(), character) != ignoredCharacters.end())
                    continue;

                // Place a road if required
                if (character == roadsObjects.symbol) {
                    PutRoad(x - 1, y - 1);
                    continue;
                }

                // Place a simple building if nothing special is required
                PutTile(x - 1, y - 1, character);
            }
            x = 0;
        }
    }

    void LevelGenerator::SpawnPlayers(const std::string& path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        if (!(levelJson.contains("player-one") && levelJson.contains("player-two")))
            return;

        //first player
        auto firstPlayerJson = levelJson["player-one"];
        glm::vec3 firstPlayerPosition = {
                firstPlayerJson["position"][0].get<float>(),
                0.3f,
                firstPlayerJson["position"][1].get<float>(),
        };
        float firstPlayerRotation = firstPlayerJson.contains("rotation") ? firstPlayerJson["rotation"].get<float>() : 0.f;
        std::string firstPlayerCarData = firstPlayerJson.contains("car-data") ? firstPlayerJson["car-data"].get<std::string>() : "res/config/cars/van.json";

        PlayerData firstPlayerData = {0, mlg::RGBA::red,
                                      firstPlayerPosition,
                                      firstPlayerRotation,
                                      firstPlayerCarData};


        //second player
        auto secondPlayerJson = levelJson["player-two"];
        glm::vec3 secondPlayerPosition = {
                secondPlayerJson["position"][0].get<float>(),
                0.3f,
                secondPlayerJson["position"][1].get<float>(),
        };
        float secondPlayerRotation = secondPlayerJson.contains("rotation") ? secondPlayerJson["rotation"].get<float>() : 0.f;
        std::string secondPlayerCarData = secondPlayerJson.contains("car-data") ? secondPlayerJson["car-data"].get<std::string>() : "res/config/cars/van.json";

        PlayerData secondPlayerData = {1, mlg::RGBA::cyan,
                                       secondPlayerPosition,
                                       secondPlayerRotation,
                                       secondPlayerCarData};

        //spawn players
        auto player = mlg::EntityManager::SpawnEntity<Player>("PlayerOne", false,
                                                              mlg::SceneGraph::GetRoot(), firstPlayerData);
        player.lock()->AddComponent<PlayerOneInput>("PlayerInput");

        auto playerTwo = mlg::EntityManager::SpawnEntity<Player>("PlayerTwo", false,
                                                                 mlg::SceneGraph::GetRoot(), secondPlayerData);
        playerTwo.lock()->AddComponent<PlayerTwoInput>("PlayerInput");
    }

    glm::vec2 LevelGenerator::GetCitySize() {
        citySize = GetLayoutSize() *= tileSize;
        return citySize;
    }

    glm::ivec2 LevelGenerator::GetLayoutSize() {
        glm::ivec2 layoutSize = glm::ivec2{0};
        for (const std::string& row : levelLayout)
            layoutSize.x = std::max(layoutSize.x, static_cast<int>(row.size()));

        layoutSize.y = static_cast<int>(levelLayout.size());

        return layoutSize;
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

        const MapObject& mapObject = mapObjectPool[mapEntry.useCount];

        float smartRotation = GetSmartRotation(x, y);
        PutEntity(mapObject, glm::ivec2{x, y}, glm::radians(smartRotation));
    }

    //TODO: Kris here - the engine does not know anything about factories
    // and now I dunno what to do :(
    //void LevelGenerator::PutFactory(int x, int y, const FactoryObject& factory) {
    //    auto factoryEntity = mlg::EntityManager::SpawnEntity<Factory>("Factory", false, mlg::SceneGraph::GetRoot(),
    //                                                                "res/levels/Factories/smelter.json");
    //    auto factoryEntityRigidBody = factoryEntity.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
    //    factoryEntityRigidBody.lock()->SetPosition({x, y});
    //}

    void LevelGenerator::PutEntity(const MapObject& mapObject, const glm::ivec2& position, float rotation) const {
        auto newEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("MapObject", !mapObject.isDynamic, mlg::SceneGraph::GetRoot()).lock();

        auto staticMesh = newEntity->AddComponent<mlg::StaticMeshComponent>("StaticMesh", mapObject.model, mapObject.material);

        glm::vec3 objectPosition{0.0f};
        objectPosition.x = -static_cast<float>(position.x) * tileSize + 0.5f * citySize.x - 0.5f * tileSize;
        objectPosition.z = -static_cast<float>(position.y) * tileSize + 0.5f * citySize.y - 0.5f * tileSize;

        newEntity->GetTransform().SetPosition(objectPosition);
        newEntity->GetTransform().SetEulerRotation({0.f, mapObject.worldRot + rotation, 0.f});
        newEntity->GetTransform().SetScale(glm::vec3{mapObject.scale});

        if (!mapObject.hasCollision)
            return;

        // add collider if needed
        auto rigidbody =
                newEntity->AddComponent<mlg::RigidbodyComponent>("Rigidbody").lock();

        std::string colType = mapObject.colliderType;
        if (colType == "rectangle") {
            rigidbody->AddCollider<mlg::ColliderShape::Rectangle>(
                    glm::vec2(glm::vec2(mapObject.colliderOffset)),
                    glm::vec2(mapObject.colliderSize));
        } else if (colType == "circle") {
            rigidbody->AddCollider<mlg::ColliderShape::Circle>(
                    glm::vec2(glm::vec2(mapObject.colliderSize)),
                    mapObject.colliderOffset);
        }

        rigidbody->SetRotation(mapObject.worldRot + rotation);
        rigidbody->SetKinematic(!mapObject.isDynamic);

        // TODO: load this from JSON? I don't know
        constexpr float timeToDestroy = 0.5f;

        if (mapObject.isDynamic) {
            newEntity->AddComponent<AutoDestroyComponent>(
                    "AutoDestroy", timeToDestroy);

            // TODO: Remove magic numbers
            rigidbody->SetLinearDrag(20.f);
            rigidbody->SetAngularDrag(10.f);
        }
    }

    float LevelGenerator::GetSmartRotation(int x, int y) {
        const Neighbours neighbours = GetNeighbours(x, y);

        float angle = 270.f;
        char symbol = roadsObjects.symbol;

        if (neighbours.left == symbol)
            angle = 270.f;
        else if (neighbours.down == symbol)
            angle = 0.f;
        else if (neighbours.right == symbol)
            angle = 90.f;
        else if (neighbours.up == symbol)
            angle = 180.f;

        return angle;
    }

    void LevelGenerator::PutRoad(int x, int y) {
        const Neighbours neighbours = GetNeighbours(x, y);

        int neighboursRoadsCount = 0;
        for (auto row : neighbours.tiles)
            for (auto tile : row)
                neighboursRoadsCount += (tile == roadsObjects.symbol ? 1 : 0);

        bool isEdge = neighboursRoadsCount < 8 && neighboursRoadsCount > 4;
        bool isCorner = neighboursRoadsCount == 8;
        bool isCross = neighboursRoadsCount == 9;

        bool isVertical = neighbours.up == roadsObjects.symbol ||
                          neighbours.down == roadsObjects.symbol;
        isVertical &= neighbours.left != roadsObjects.symbol &&
                      neighbours.right != roadsObjects.symbol;

        bool isHorizontal = neighbours.left == roadsObjects.symbol ||
                            neighbours.right == roadsObjects.symbol;
        isHorizontal &= neighbours.up != roadsObjects.symbol &&
                        neighbours.down != roadsObjects.symbol;

        if (isEdge) {
            PutEdgeRoad(x, y);
        } else if (isCorner) {
            PutCornerRoad(x, y);
        } else if (isCross) {
            PutCrossRoad(x, y);
        } else if (isVertical) {
            PutStraightRoad(x, y, false);
        } else if (isHorizontal) {
            PutStraightRoad(x, y, true);
        } else {
            PutCurveRoad(x, y);
        }
    }

    void LevelGenerator::PutCrossRoad(int x, int y) {
        PutEntity(roadsObjects.cross, glm::ivec2{x, y}, 0.f);
    }

    void LevelGenerator::PutStraightRoad(int x, int y, bool isHorizontal) {
        float angle = isHorizontal ? 90.f : 0.f;
        auto mapObject = roadsObjects.road;

        PutEntity(roadsObjects.road, glm::ivec2{x, y}, glm::radians(angle));
    }

    void LevelGenerator::PutEdgeRoad(int x, int y) {
        float angle;
        const Neighbours neighbours = GetNeighbours(x, y);

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

    void LevelGenerator::PutCornerRoad(int x, int y) {
        float angle;
        const Neighbours neighbours = GetNeighbours(x, y);

        if (neighbours.leftUp != roadsObjects.symbol)
            angle = 0.f;
        else if (neighbours.leftDown != roadsObjects.symbol)
            angle = 90.f;
        else if (neighbours.rightDown != roadsObjects.symbol)
            angle = 180.f;
        else
            angle = 270.f;

        PutEntity(roadsObjects.corner, glm::ivec2{x, y}, glm::radians(angle));
    }

    void LevelGenerator::PutCurveRoad(int x, int y) {
        float angle = 45;
        const Neighbours neighbours = GetNeighbours(x, y);

        if (neighbours.up == neighbours.right && neighbours.up == roadsObjects.symbol)
            angle = 0.f;
        else if (neighbours.up == neighbours.left && neighbours.up == roadsObjects.symbol)
            angle = 90.f;
        else if (neighbours.down == neighbours.left && neighbours.down == roadsObjects.symbol)
            angle = 180.f;
        else
            angle = 270.f;

        PutEntity(roadsObjects.curve, glm::ivec2{x, y}, glm::radians(angle));
    }

    Neighbours LevelGenerator::GetNeighbours(int x, int y) {
        Neighbours result;

        for (int i = -1; i < 2; ++i) {
            for (int j = -1; j < 2; ++j) {
                result.tiles[i + 1][j + 1] = GetTileOrZero(x + j, y + i);
            }
        }

        return result;
    }

    char LevelGenerator::GetTileOrZero(int x, int y) {
        if (x < 0 || y < 0)
            return 0;

        if (y > levelLayout.size() - 1)
            return 0;

        if (x > levelLayout[y].size() - 1)
            return 0;

        return levelLayout[y][x];
    }

}// namespace mlg