#pragma once
#include "pch.h"
#include "Rendering/Model.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"
#include "MapObject.h"

namespace mlg {
    using json = nlohmann::json;

    class LevelGenerator {
    public:

        static void Initialize();
        static void Stop();
        static LevelGenerator* GetInstance();

        static std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<MapObject>>> LoadJson(const std::string& path);
        static void GenerateLevel();
        static bool GenerateTestLevel();

    private:
        static LevelGenerator* instance;
        LevelGenerator() = default;

        static std::vector<std::vector<char>> levelLayout;

        static void PutObject(const std::string& modelPath, const std::string& materialPath, glm::vec3 pos, float rotation = 0.0f);
        static void PutObject(std::shared_ptr<ModelAsset>& modelAsset, std::shared_ptr<MaterialAsset>& materialAsset, glm::vec3 pos, float rotation = 0.0f);
        static void PutObject(const std::shared_ptr<MapObject>& obj, glm::vec3 pos);
        static std::string Hash(const std::string& hashString, float posX, float posY);
    };

} //mlg
 