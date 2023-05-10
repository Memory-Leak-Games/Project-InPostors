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
        struct MapEntry {
            std::vector<std::shared_ptr<class MapObject>> object;
            int useCount = 0;
        };

        static LevelGenerator* instance;

        std::string defaultLevelMaterial;
        std::vector<std::string> levelLayout;
        std::unique_ptr<std::unordered_map<char, MapEntry>> mapObjects;
        float tileSize = 10.0f;

        LevelGenerator() = default;
        ~LevelGenerator() = default;

        void PutObject(const std::shared_ptr<class MapObject>& obj, glm::vec3 pos);
        static std::string Hash(const std::string& hashString, float posX, float posY);

        std::shared_ptr<MapObject> ParseObject(nlohmann::json jsonObject);
    };

} //mlg
 