#pragma once

namespace mlg {

    class LevelGenerator {

    public:

        static void Initialize();
        static void Stop();
        static LevelGenerator* GetInstance();

        void LoadJson(const std::string& path);
        void LoadLayout(const std::string& path);
        void LoadMapObjects(const std::string &path);
        void GenerateLevel();

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

        struct MapEntry {
            const std::vector<MapObject> object;
            int useCount = 0;
        };

        static LevelGenerator* instance;

        std::string defaultLevelMaterial;
        std::vector<std::string> levelLayout;
        std::unordered_map<char, MapEntry> mapObjects;
        float tileSize = 10.0f;

        LevelGenerator() = default;
        ~LevelGenerator() = default;

        void PutObject(const class MapObject& obj, glm::vec3 pos);
        static std::string Hash(const std::string& hashString, float posX, float posY);

        MapObject ParseObject(nlohmann::json jsonObject);
    };

} //mlg
 