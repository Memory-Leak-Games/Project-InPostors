#pragma once

namespace mlg {

    class LevelGenerator {

    public:
        static void LoadMap(const std::string& path);
        static void LoadCameraSettings(const std::string& path, class CameraComponent& cameraComponent);
        static void SpawnGround(const std::string& path);

    private:

        struct MapObject {
            std::shared_ptr<class ModelAsset> model;
            std::shared_ptr<class MaterialAsset> material;

            float worldRot;
            float scale;

            bool hasCollision = false;
            std::string colliderType;
            float colliderSize = 1.0f;
            float colliderOffset = 0.0f;
        };

        struct Roads {
            char symbol;
            MapObject road;
            MapObject edge;
            MapObject cross;
            MapObject curve;
        } roadsObjects;

        struct MapEntry {
            std::vector<MapObject> objectsPool;
            bool isPathWay = false;

            int useCount = 0;
        };

        struct Neighbours {
            char left = 0;
            char down = 0;
            char right = 0;
            char up = 0;
        };

        LevelGenerator() = default;
        ~LevelGenerator() = default;

        nlohmann::json levelJson;

        std::vector<std::string> levelLayout;
        std::unordered_map<char, MapEntry> mapObjects;

        std::string defaultMaterial;
        float tileSize;
        glm::vec2 citySize;

        void LoadLayout();
        void LoadMapObjects();
        void LoadRoads();
        MapObject ParseObject(const nlohmann::json& jsonObject);

        void GenerateLevel();

        void PutTile(int x, int y, const char& character);
        void PutRoad(int x, int y);

        void PutStraightRoad(int x, int y, bool isVertical);
        void PutEdgeRoad(int x, int y);
        void PutCrossRoad(int x, int y);
        void PutCurveRoad(int x, int y);

        void PutEntity(const MapObject& mapObject, const glm::ivec2& position, float rotation);
        float GetSmartRotation(const glm::ivec2& layoutPosition);

        glm::vec2 GetCitySize();
        Neighbours GetNeighbours(int x, int y);
    };

} //mlg
 