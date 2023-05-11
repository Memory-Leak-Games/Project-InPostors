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
            MapObject vertical;
            MapObject horizontal;
            MapObject cross;
            MapObject curve;
        } roadsObjects;

        struct MapEntry {
            std::vector<MapObject> objectsPool;
            bool isPathWay = false;

            int useCount = 0;
        };

        LevelGenerator() = default;
        ~LevelGenerator() = default;

        nlohmann::json levelJson;

        std::vector<std::string> levelLayout;
        std::unordered_map<char, MapEntry> mapObjects;

        std::string defaultMaterial;
        float tileSize;

        void LoadLayout();
        void LoadMapObjects();
        void LoadRoads();
        MapObject ParseObject(const nlohmann::json& jsonObject);

        void GenerateLevel();

        void PutTile(const glm::vec2& citySize, int x, int y, const char& character);
        void PutRoad(const glm::vec2& citySize, int x, int y);

        void PutEntity(const MapObject& mapObject, const glm::vec3& position, float rotation);
        float GetSmartRotation(const glm::ivec2& layoutPosition);

        glm::vec2 GetCitySize();
    };

} //mlg
 