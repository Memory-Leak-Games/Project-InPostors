#pragma once

// TODO: This class should be a singleton not a static class
//      Static classes are used for set of methods (like math lib) rather than Super class

namespace mlg {

    class LevelGenerator {

    public:

        static void Initialize();
        static void Stop();
        static LevelGenerator* GetInstance();

        void LoadJson(const std::string& path);
        void GenerateLevel(float tileSize = 10.0f);

    private:
        struct MapEntry {
            std::vector<std::shared_ptr<class MapObject>> object;
            int useCount = 0;
        };

        static LevelGenerator* instance;

        // TODO: Does it have to be a member of the class?
        std::vector<std::string> levelLayout;

        std::unique_ptr<std::unordered_map<char, MapEntry>> mapObjects;

        LevelGenerator() = default;
        //~LevelGenerator() = default;

        void PutObject(const std::shared_ptr<class MapObject>& obj, glm::vec3 pos);
        static std::string Hash(const std::string& hashString, float posX, float posY);
    };

} //mlg
 