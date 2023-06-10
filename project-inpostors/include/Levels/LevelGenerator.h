#pragma once

#include "Neighbours.h"
#include "Managers/TaskManager.h"

namespace mlg {

    class LevelGenerator {

    public:
        struct TrafficData{
            uint32_t numberOfAgents = 0;
        };

        //todo: rewrite these functions so that you need to provide json path only once
        static std::vector<std::string> LoadMap(const std::string& path);
        static void LoadCameraSettings(const std::string& path, class CameraComponent& cameraComponent);
        static void SpawnGround(const std::string& path);
        static void SetCityBounds(const std::string& path);
        static void SpawnPlayers(const std::string& path);
        static TrafficData LoadTrafficData(const std::string& path);
        static std::string LoadLevelName(const std::string& path);
        static float LoadLevelTimeLimit(const std::string& path);
        //todo: i really need to separate level generator from level data...
        static std::vector<TaskData> GetTasks(const std::string& path);

        std::vector<std::string> GetLevelLayout();

    private:
        struct MapObject {
            std::string modelPath;
            std::string materialPath;
            std::string colliderType;
            float worldRot;
            float scale;
            float colliderSize = 1.0f;
            float colliderOffset = 0.0f;
            float lifetime = -1.f; // Only valid when isDynamic is set to true
            float linearDrag = 20.f;
            float angularDrag = 10.f;
            bool hasCollision = false;
            bool isDynamic = false;
        };

        struct Roads {
            MapObject road;
            MapObject edge;
            MapObject cross;
            MapObject curve;
            MapObject corner;
            char symbol = ' ';
        } roadsObjects;

        //TODO: more info about pools?
        struct MapFactory {
            std::string configPath;
            unsigned int remaining;
            char factorySymbol;
            char fallbackSymbol;
        };

        struct MapEntry {
            std::vector<MapObject> objectsPool;
            int useCount = 0;
            bool isPathWay = false;
        };

        LevelGenerator() = default;
        ~LevelGenerator() = default;

        nlohmann::json levelJson;
        nlohmann::json tileJson;
        nlohmann::json poolJson;

        std::vector<std::string> levelLayout;
        std::vector<MapFactory> levelFactories;

        std::string ignoredCharacters;
        std::unordered_map<char, unsigned int> factoryCharacters;
        std::unordered_map<char, MapEntry> mapObjects;
        std::string defaultMaterial;

        float tileSize = 10.f;

        // ======== METHODS ========

        std::vector<std::string> LoadLayout();
        void LoadMapObjects();
        void LoadFactories();
        void LoadRoads();

        MapObject ParseObject(const nlohmann::json& jsonObject);
        MapFactory ParseFactory(const nlohmann::json& jsonObject);

        void GenerateLevel();

        void TryPutFactory(const char& character, const glm::vec2& pos); //TODO: move code from GenerateLevel here

        void PutTile(int x, int y, const char& character);
        void PutRoad(int x, int y);

        void PutStraightRoad(int x, int y, bool isVertical);
        void PutEdgeRoad(int x, int y);
        void PutCrossRoad(int x, int y);
        void PutCurveRoad(int x, int y);
        void PutCornerRoad(int x, int y);

        void PutEntity(const MapObject& mapObject, const glm::ivec2& position, float rotation) const;
        void PutFactory(const std::string& configPath, const glm::ivec2& position,
                        float rotation = 0.f) const;

        float GetSmartRotation(int x, int y);

        [[nodiscard]] glm::vec2 GetCitySize() const;
        [[nodiscard]] glm::ivec2 GetLayoutSize() const;

        Neighbours GetNeighbours(int x, int y);
        char GetTileOrZero(int x, int y);

        [[nodiscard]] glm::vec3 GetLevelPosition(const glm::ivec2 &localPos,
                                                 bool isRigid = false) const;
    };

}// namespace mlg
