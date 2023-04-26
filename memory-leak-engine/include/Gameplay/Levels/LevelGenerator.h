#pragma once

// TODO: HINT do not add includes in header, because when you change one, you need to
//  recompile every other file that includes these

// TODO: This class should be a singleton not a static class
//      Static classes are used for set of methods (like math lib) rather than Super class

namespace mlg {

    class LevelGenerator {

    public:

        static void Initialize();
        static void Stop();
        static LevelGenerator* GetInstance();

        static void LoadJson(const std::string& path);
        static void GenerateLevel(float tileSize = 10.0f);
        static bool GenerateTestLevel();

    private:
        static LevelGenerator* instance;

        // TODO: I changed type to string to simplify typing (string is fancy class for vector of characters)
        // TODO: And does it have to be a member of the class?
        static std::vector<std::string> levelLayout;

        static std::unique_ptr<std::unordered_map<char,
            std::vector<std::shared_ptr<class MapObject>>>> mapObjects;

        LevelGenerator() = default;
        //~LevelGenerator() = default;

        static void PutObject(std::shared_ptr<class ModelAsset>& modelAsset, std::shared_ptr<class MaterialAsset>& materialAsset,
                              glm::vec3 pos, float rotation = 0.0f);
        static void PutObject(const std::shared_ptr<class MapObject>& obj, glm::vec3 pos);
        static std::string Hash(const std::string& hashString, float posX, float posY);
    };

} //mlg
 