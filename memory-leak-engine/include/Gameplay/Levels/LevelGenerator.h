#pragma once
#include "pch.h"
#include "Rendering/Model.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"
#include "LevelLayout.h"
#include "MapObject.h"

namespace mlg {

    class LevelGenerator {
    public:
        static bool GenerateLevel(const mlg::LevelLayout& layout);
        static bool GenerateTestLevel();

    private:
        //void CreateMapObject()
        static void PutObject(const std::string& modelPath, const std::string& materialPath, glm::vec3 pos, float rotation = 0.0f);
        static void PutObject(std::shared_ptr<ModelAsset>& modelAsset, std::shared_ptr<MaterialAsset>& materialAsset, glm::vec3 pos, float rotation = 0.0f);
        static void PutObject(const std::shared_ptr<MapObject>& obj, glm::vec3 pos);
        static std::string Hash(const std::string& hashString, float posX, float posY);
    };

} //mlg
 