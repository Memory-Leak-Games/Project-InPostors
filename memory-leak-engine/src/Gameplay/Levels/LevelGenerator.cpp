#include "Gameplay/Levels/LevelGenerator.h"
#include "Gameplay/EntityManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/Components/StaticMeshComponent.h"
#include "SceneGraph/SceneGraph.h"
#include "Macros.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Physics/Colliders/ColliderShapes.h"
#include <fstream>

#include "Rendering/Model.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"
#include "effolkronium/random.hpp"
#include "nlohmann/json.hpp"

#include "Gameplay/Levels/MapObject.h"

using json = nlohmann::json;
using Random = effolkronium::random_static;

namespace mlg {
    LevelGenerator *LevelGenerator::instance;


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

    LevelGenerator *LevelGenerator::GetInstance() {
        return instance;
    }


    void LevelGenerator::LoadJson(const std::string &path) {
        LoadLayout(path);
        LoadMapObjects(path);
    }


    void LevelGenerator::LoadLayout(const std::string &path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);

        if (!levelLayout.empty())
            levelLayout.clear();

        for (const auto &jsonLayoutString: levelJson["layout"]) {
            levelLayout.push_back(jsonLayoutString.get<std::string>());
        }
        tileSize = levelJson.contains("tile-size")
                   ? levelJson["tile"].get<float>() : 10.0f;
    }


    void LevelGenerator::LoadMapObjects(const std::string &path) {
        std::ifstream levelFile{path};
        json levelJson = json::parse(levelFile);
        // TODO: Kris here - for now I have no idea how to prevent loading JSON twice.
        //  If you have any idea how to do it without including json hpp in header file, hit me up :)

        if (!mapObjects->empty())
            mapObjects->clear();

        mapObjects = std::make_unique<std::unordered_map<char, LevelGenerator::MapEntry>>();

        for (const auto &jsonTile: levelJson["tiles"]) {
            const char tileSymbol = jsonTile["symbol"].get<std::string>()[0];
            std::vector<std::shared_ptr<MapObject>> mapObjectPool;

            //TODO: move this to a separate function maybe?
            for (const auto &jsonMapObject: jsonTile["objects"]) {
                std::string modelPath = jsonMapObject["model"].get<std::string>();
                std::string materialPath = jsonMapObject.contains("material")
                                           ? jsonMapObject["material"].get<std::string>()
                                           : "res/models/Buildings/buildings_material.json";
                float yRot = jsonMapObject.contains("rotation")
                             ? jsonMapObject["rotation"].get<float>() : 0.0f;

                mlg::MapObject mapObj(modelPath, materialPath, yRot);
                if (jsonMapObject.contains("collision-type")) {
                    std::string cType = jsonMapObject["collision-type"].get<std::string>();
                    float cSize = jsonMapObject.contains("collision-size")
                                  ? jsonMapObject["collision-size"].get<float>() : 1.0f;
                    float cOffset = jsonMapObject.contains("collision-offset")
                                    ? jsonMapObject["collision-offset"].get<float>() : 0.0f;
                    mapObj.AddCollision(cType, cSize, cOffset);
                }

                auto newMapObj = std::make_shared<MapObject>(mapObj);
                mapObjectPool.push_back(newMapObj);
            }
            mapObjects->insert({tileSymbol, {std::move(mapObjectPool), 0}});
        }
    }


    void LevelGenerator::GenerateLevel() {
        glm::vec2 citySize{0.f};
        citySize.y = static_cast<float>(levelLayout.size());

        for (const std::string &row: levelLayout) {
            citySize.x = std::max(citySize.x, static_cast<float>(row.size()));
        }

        citySize *= tileSize;

        int i = 0, j = 0;
        for (const std::string &row: levelLayout) {
            ++j;
            for (const char &character: row) {
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


    void LevelGenerator::PutObject(const std::shared_ptr<MapObject> &obj, glm::vec3 pos) {
        auto modelEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
                Hash("MapObject", pos.x, pos.z),
                true, mlg::SceneGraph::GetRoot());
        modelEntity.lock()->AddComponent<mlg::StaticMeshComponent>(
                "StaticMesh", obj->GetModel().lock(), obj->GetMaterial().lock());
        modelEntity.lock()->GetTransform().SetPosition(pos);
        modelEntity.lock()->GetTransform().SetEulerRotation(obj->GetRotation());

        // add collider if needed
        if (!obj->HasCollision())
            return;

        auto rb = modelEntity.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        std::string colType = obj->GetColliderType();
        if (colType == "rectangle") {
            rb.lock()->AddCollider<mlg::ColliderShape::Rectangle>(
                    glm::vec2(glm::vec2(obj->GetColliderOffset())),
                    glm::vec2(obj->GetColliderSize()));
        } else if (colType == "circle") {
            rb.lock()->AddCollider<mlg::ColliderShape::Circle>(
                    glm::vec2(glm::vec2(obj->GetColliderSize())),
                    obj->GetColliderOffset());
        }
        rb.lock()->SetRotation(obj->GetRotation().y);
    }


    std::string LevelGenerator::Hash(const std::string &hashString, float posX, float posY) {
        std::size_t h1 = std::hash<std::string>{}(hashString);
        auto h2 = static_cast<size_t>(posX * posY + posX);
        std::stringstream ss;
        ss << (h1 ^ (h2 << 1));
        return ss.str();
    }
}