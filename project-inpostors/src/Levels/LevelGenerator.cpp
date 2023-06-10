#include <effolkronium/random.hpp>
#include <fstream>
#include <memory>
#include <utility>

#include "Core/AssetManager/AssetManager.h"
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
#include "Buildings/Factory.h"
#include "Managers/TaskManager.h"


using json = nlohmann::json;
using Random = effolkronium::random_static;

namespace mlg {

    std::vector<std::string> LevelGenerator::LoadMap(const std::string &path) {
        LevelGenerator levelGenerator;

        std::ifstream levelFile{path};
        levelGenerator.levelJson = json::parse(levelFile);

        std::vector<std::string> ret = levelGenerator.LoadLayout();

        levelGenerator.LoadMapObjects();
        levelGenerator.LoadFactories();
        levelGenerator.LoadRoads();

        levelGenerator.GenerateLevel();

        if (!levelGenerator.levelJson.contains("sublevels"))
            return ret;

        for (const auto &sublevel: levelGenerator.levelJson["sublevels"]) {
            LoadMap(sublevel);
        }

        levelFile.close();

        return ret;
    }

    void LevelGenerator::LoadCameraSettings(const std::string &path, struct CameraComponent &cameraComponent) {
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

        levelFile.close();
    }

    void LevelGenerator::SpawnGround(const std::string &path) {
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

        ground.lock()->GetTransform().SetScale(groundScale * 10.f);

        levelFile.close();
    }

    void LevelGenerator::SetCityBounds(const std::string &path) {
        LevelGenerator levelGenerator;
        std::ifstream levelFile{path};

        levelGenerator.levelJson = json::parse(levelFile);
        levelGenerator.LoadLayout();

        const glm::vec2 citySize = levelGenerator.GetCitySize();
        const glm::ivec2 layoutSize = levelGenerator.GetLayoutSize();

        const glm::vec2 cityStart = -0.5f * citySize;
        const glm::vec2 cityEnd = 0.5f * citySize;

        CollisionManager::SetBounds(cityStart, cityEnd, layoutSize);

        levelFile.close();
    }

    void LevelGenerator::SpawnPlayers(const std::string &path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        if (!(levelJson.contains("player-one") && levelJson.contains("player-two"))) {
            levelFile.close();
            return;
        }

        auto playerJsons = {levelJson["player-one"], levelJson["player-two"]};
        std::vector<std::weak_ptr<Entity>> players;

        int i = 0;

        for (const auto &playerJson: playerJsons) {
            glm::vec2 position = {playerJson["position"][0], playerJson["position"][1]};

            float rotation = playerJson.value("rotation", 0.f);
            std::string data = playerJson.value("car-data", "res/config/cars/van.json");

            int id = playerJson.value("id", i);
            glm::vec4 color = {
                    playerJson["color"][0],
                    playerJson["color"][1],
                    playerJson["color"][2],
                    1.f};

            PlayerData playerData = {id, color, data};
            
            std::string playerName =  fmt::format("Player{}", i + 1);

            auto playerEntity = mlg::EntityManager::SpawnEntity<Player>(
                    playerName, false, mlg::SceneGraph::GetRoot(), playerData);

            auto player = std::dynamic_pointer_cast<Player>(playerEntity.lock());
            player->SetPlayerPosition(position);
            player->SetPlayerRotation(rotation);

            players.push_back(playerEntity);

            i++;
        }

        players[0].lock()->AddComponent<PlayerOneInput>("PlayerInput");
        players[1].lock()->AddComponent<PlayerTwoInput>("PlayerInput");

        levelFile.close();
    }

    LevelGenerator::TrafficData LevelGenerator::LoadTrafficData(const std::string &path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        TrafficData trafficData;
        trafficData.numberOfAgents = levelJson.value("number-of-agents", 0);

        levelFile.close();

        return trafficData;
    }

    std::string LevelGenerator::LoadLevelName(const std::string &path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        levelFile.close();

        return levelJson["name"];
    }


    float LevelGenerator::LoadLevelTimeLimit(const std::string &path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        return levelJson.value("time-limit", -1.f);
    }

    std::vector<TaskData> LevelGenerator::GetTasks(const std::string &path) {
        std::vector<TaskData> tasks = {};

        // This is straight up stupid. But it works.
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        std::ifstream tileFile{levelJson["tileset"].get<std::string>()};
        json tileJson = json::parse(tileFile);

        std::ifstream poolFile{tileJson["factory-pool"].get<std::string>()};
        json poolJson = json::parse(poolFile);

        for (const auto &jsonTask: poolJson["tasks"]) {
            int quant = jsonTask["quantity"].get<int>();
            tasks.reserve(quant);
            TaskData newTask;
            newTask.productId = jsonTask["product-id"].get<std::string>();
            newTask.time = jsonTask["time"].get<float>();
            newTask.reward = jsonTask["reward"].get<int>();
            newTask.bonus = jsonTask["bonus"].get<int>();

            for (int i = 0; i < quant; ++i)
                tasks.push_back(newTask);
        }

        poolFile.close();
        tileFile.close();
        levelFile.close();

        return tasks;
    }


    std::vector<std::string> LevelGenerator::GetLevelLayout() {
        return levelLayout;
    }

    // ======== PRIVATE METHODS ========

    std::vector<std::string> LevelGenerator::LoadLayout() {
        std::vector<std::string> ret;
        for (const auto &jsonLayoutString: levelJson["layout"]) {
            std::string layoutString = jsonLayoutString.get<std::string>();
            levelLayout.push_back(layoutString);
            ret.push_back(layoutString);
        }

        tileSize = levelJson["tile-size"].get<float>();
        return ret;
    }

    void LevelGenerator::LoadMapObjects() {
        std::ifstream tileFile{levelJson["tileset"].get<std::string>()};
        tileJson = json::parse(tileFile);

        defaultMaterial = tileJson["default-material"].get<std::string>();

        for (const auto &jsonTile: tileJson["tiles"]) {
            const char tileSymbol = jsonTile["symbol"].get<std::string>()[0];

            if (tileSymbol == roadsObjects.symbol)
                continue;

            std::vector<MapObject> mapObjectPool;

            bool isPathWay = false;
            if (jsonTile.contains("is-path-way")) {
                isPathWay = jsonTile["is-path-way"].get<bool>();
            }

            for (const auto &jsonMapObject: jsonTile["objects"]) {
                mapObjectPool.push_back(std::move(ParseObject(jsonMapObject)));
            }

            mapObjects.insert({tileSymbol, MapEntry{mapObjectPool, isPathWay, false}});
        }

        if (levelJson.contains("ignore"))
            ignoredCharacters = levelJson["ignore"].get<std::string>();

        tileFile.close();
    }

    void LevelGenerator::LoadFactories() {
        if (!tileJson.contains("factory-pool"))
            return;

        std::ifstream factoryPool{tileJson["factory-pool"].get<std::string>()};
        poolJson = json::parse(factoryPool);

        // Assuming we have 4 tiers of factories,
        // we need to keep 4 symbols.
        // TODO: increase size if we happen to have move factories in pool
        factoryCharacters.reserve(4);
        for (const auto &jsonFactory: poolJson["factories"]) {
            MapFactory factory = ParseFactory(jsonFactory);
            factory.remaining += 1;
            levelFactories.push_back(factory);
        }

        factoryPool.close();
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

    LevelGenerator::MapObject LevelGenerator::ParseObject(const json &jsonMapObject) {
        MapObject mapObj;
        mapObj.modelPath = jsonMapObject["model"];
        mapObj.materialPath = jsonMapObject.value("material", defaultMaterial);

        mapObj.worldRot = jsonMapObject.value("rotation", 0.f);
        mapObj.worldRot = glm::radians(mapObj.worldRot);

        mapObj.scale = jsonMapObject.value("scale", 1.f);

        if (!jsonMapObject.contains("collision-type"))
            return mapObj;

        mapObj.hasCollision = true;
        mapObj.colliderType = jsonMapObject["collision-type"];
        mapObj.colliderSize = jsonMapObject.value("collision-size", 1.f);
        mapObj.colliderOffset = jsonMapObject.value("collision-offset", 0.f);
        mapObj.isDynamic = jsonMapObject.value("dynamic", false);
        if (!mapObj.isDynamic)
            return mapObj;

        mapObj.lifetime = jsonMapObject.value("lifetime", 0.5f);
        mapObj.linearDrag = jsonMapObject.value("linear-drag", 20.0f);
        mapObj.angularDrag = jsonMapObject.value("angular-drag", 10.0f);

        return mapObj;
    }

    LevelGenerator::MapFactory LevelGenerator::ParseFactory(const nlohmann::json &jsonObject) {
        const char tileSymbol = jsonObject["symbol"].get<std::string>()[0];

        // Count factory occurrences.
        // This is needed for random generation chance.
        int x = 0, y = 0;
        unsigned int occurrences = 0;

        for (const std::string &row: levelLayout) {
            ++y;
            for (const char &character: row) {
                ++x;
                if (character == tileSymbol)
                    ++occurrences;
            }
            x = 0;
        }
        factoryCharacters.insert({tileSymbol, occurrences});

        MapFactory mf;
        mf.configPath = jsonObject["config"].get<std::string>();
        mf.factorySymbol = tileSymbol;
        mf.fallbackSymbol = jsonObject["fallback"].get<std::string>()[0];

        return mf;
    }

    void LevelGenerator::GenerateLevel() {
        int x = 0, y = 0;
        for (const std::string &row: levelLayout) {
            ++y;
            for (const char &character: row) {
                ++x;
                if (factoryCharacters.find(character) != factoryCharacters.end()) {// we got a match!
                    TryPutFactory(character, {x, y});
                    continue;
                }

                // Ignore spaces
                if (character == ' ')
                    continue;

                // Ignore specified characters
                if (std::find(ignoredCharacters.begin(), ignoredCharacters.end(),
                              character) != ignoredCharacters.end())
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

    void LevelGenerator::TryPutFactory(const char &character, const glm::vec2 &pos) {
        for (auto &fac: levelFactories) {
            if (fac.factorySymbol == character && fac.remaining != 0) {
                unsigned int spots = factoryCharacters[character];
                float chance = static_cast<float>(fac.remaining) / static_cast<float>(spots);
                if (chance > 1.f) {
                    SPDLOG_WARN("More factories in pool than allowed to spawn.");
                    chance = 1.f;
                }

                if (Random::get<bool>(chance)) {
                    PutFactory(fac.configPath, {pos.x - 1, pos.y - 1}, 0.0f);
                    fac.remaining -= 1;
                } else if (fac.fallbackSymbol != ' ')
                    PutTile(static_cast<int>(pos.x) - 1,
                            static_cast<int>(pos.y) - 1,
                            fac.fallbackSymbol);

                factoryCharacters[character] -= 1;
            }
        }
    }

    void LevelGenerator::PutTile(int x, int y, const char &character) {
        if (!mapObjects.contains(character)) {
            SPDLOG_WARN("LevelLoader: unknown character: {}, at ({}, {}).", character, x, y);
            return;
        }

        MapEntry &mapEntry = mapObjects.at(character);
        std::vector<MapObject> &mapObjectPool = mapEntry.objectsPool;

        mapEntry.useCount++;

        if (mapEntry.useCount > mapObjectPool.size() - 1) {
            mapEntry.useCount = 0;
            Random::shuffle(mapObjectPool);
        }

        const MapObject &mapObject = mapObjectPool[mapEntry.useCount];

        float smartRotation = GetSmartRotation(x, y);
        PutEntity(mapObject, glm::ivec2{x, y},
                  glm::radians(smartRotation));
    }

    void LevelGenerator::PutRoad(int x, int y) {
        const Neighbours neighbours = GetNeighbours(x, y);

        int neighboursRoadsCount = 0;
        for (auto row: neighbours.tiles)
            for (auto tile: row)
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

    void LevelGenerator::PutStraightRoad(int x, int y, bool isHorizontal) {
        float angle = isHorizontal ? 90.f : 0.f;
        auto mapObject = roadsObjects.road;

        PutEntity(roadsObjects.road, glm::ivec2{x, y},
                  glm::radians(angle));
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

        PutEntity(roadsObjects.edge, glm::ivec2{x, y},
                  glm::radians(angle));
    }

    void LevelGenerator::PutCrossRoad(int x, int y) {
        PutEntity(roadsObjects.cross, glm::ivec2{x, y}, 0.f);
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

    void LevelGenerator::PutEntity(const MapObject &mapObject, const glm::ivec2 &position, float rotation) const {
        auto newEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
                "MapObject", !mapObject.isDynamic, mlg::SceneGraph::GetRoot())
                .lock();

        auto model = mlg::AssetManager::GetAsset<ModelAsset>(mapObject.modelPath);
        auto material = mlg::AssetManager::GetAsset<MaterialAsset>(mapObject.materialPath);

        auto staticMesh = newEntity->AddComponent<mlg::StaticMeshComponent>("StaticMesh", model, material);

        glm::vec3 objectPosition = GetLevelPosition(position);

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
                    glm::vec2(mapObject.colliderOffset),
                    glm::vec2(mapObject.colliderSize));
        } else if (colType == "circle") {
            rigidbody->AddCollider<mlg::ColliderShape::Circle>(
                    glm::vec2(glm::vec2(mapObject.colliderSize)),
                    mapObject.colliderOffset);
        }

        rigidbody->SetRotation(mapObject.worldRot + rotation);
        rigidbody->SetKinematic(!mapObject.isDynamic);

        if (mapObject.isDynamic) {
            newEntity->AddComponent<AutoDestroyComponent>(
                    "AutoDestroy", mapObject.lifetime);

            rigidbody->SetLinearDrag(mapObject.linearDrag);
            rigidbody->SetAngularDrag(mapObject.angularDrag);
        }
    }

    void LevelGenerator::PutFactory(const std::string &configPath, const glm::ivec2 &position,
                                    float rotation) const {
        auto factory = mlg::EntityManager::SpawnEntity<Factory>("Factory", false, mlg::SceneGraph::GetRoot(),
                                                                configPath);
        auto factoryRigidBody = factory.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");

        glm::vec2 factoryPosition = GetLevelPosition(position, true);

        factoryRigidBody.lock()->SetPosition(factoryPosition);
        factoryRigidBody.lock()->SetRotation(glm::radians(rotation));
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

    glm::vec2 LevelGenerator::GetCitySize() const {
        return GetLayoutSize() *= tileSize;
    }

    glm::ivec2 LevelGenerator::GetLayoutSize() const {
        glm::ivec2 layoutSize = glm::ivec2{0};
        for (const std::string &row: levelLayout)
            layoutSize.x = std::max(layoutSize.x, static_cast<int>(row.size()));

        layoutSize.y = static_cast<int>(levelLayout.size());

        return layoutSize;
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

    glm::vec3 LevelGenerator::GetLevelPosition(const glm::ivec2 &localPos, bool isRigid) const {
        glm::vec3 levelPos = {0.f, 0.f, 0.f};
        glm::vec2 citySize = GetCitySize();
        levelPos.x = -static_cast<float>(localPos.x) * tileSize + 0.5f * citySize.x - 0.5f * tileSize + 0.5f;
        float locPos = -static_cast<float>(localPos.y) * tileSize + 0.5f * citySize.y - 0.5f * tileSize - 0.5f;
        if (!isRigid)
            levelPos.z = locPos;
        else
            levelPos.y = locPos;

        return levelPos;
    }

}// namespace mlg
