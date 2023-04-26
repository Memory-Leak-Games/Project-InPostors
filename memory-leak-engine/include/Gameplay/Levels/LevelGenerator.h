#pragma once

// TODO: HINT do not add includes in header, because when you change one, you need to
//  recompile every other file that includes these

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

        static int levelWidth;
        static int levelHeight;
        static std::vector<std::vector<char>> levelLayout;
        static std::unique_ptr<std::unordered_map<std::string,
                std::vector<std::shared_ptr<class MapObject>>>> mapObjects;

        LevelGenerator() = default;
        //~LevelGenerator() = default;

        static void PutObject(std::shared_ptr<class ModelAsset>& modelAsset, std::shared_ptr<class MaterialAsset>& materialAsset,
                              glm::vec3 pos, float rotation = 0.0f);
        static void PutObject(const std::shared_ptr<class MapObject>& obj, glm::vec3 pos);
        static std::string Hash(const std::string& hashString, float posX, float posY);
    };

} //mlg
 