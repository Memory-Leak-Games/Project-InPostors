#pragma once

namespace mlg {

    class LevelGenerator {

    public:
        static void LoadMap(const std::string& path);

    private:
        enum class RoadType {
            None,
            Cross,
            Vertical,
            Horizontal
        };

        struct MapObject {
            std::shared_ptr<class ModelAsset> model;
            std::shared_ptr<class MaterialAsset> material;

            float worldRot;
            float scale;

            bool isCorner = false;

            bool hasCollision = false;
            std::string colliderType;
            float colliderSize = 1.0f;
            float colliderOffset = 0.0f;
        };

        struct MapEntry {
            RoadType roadType = RoadType::None;
            std::vector<MapObject> objectsPool;

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
        MapObject ParseObject(const nlohmann::json& jsonObject);

        void GenerateLevel();
        void PutObject(const MapObject& mapObject, const glm::vec3& position, float rotation);

        float GetSmartRotation(const glm::ivec2& layoutPosition);
    };

} //mlg
 