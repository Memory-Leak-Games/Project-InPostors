#pragma once
#include "pch.h"
#include "Rendering/Model.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

namespace mlg {

    class LevelGenerator {
    public:
        static bool GenerateLevel();
        static bool GenerateTestLevel();

    private:
        static bool PutObject(const std::string& modelPath, const std::string& materialPath, glm::vec3 pos, float rotation = 0.0f);
        static bool PutObject(std::shared_ptr<ModelAsset>& modelAsset, std::shared_ptr<MaterialAsset>& materialAsset, glm::vec3 pos, float rotation = 0.0f);
        static std::string Hash(const std::string& modelPath, float posX, float posY);
    };

} //mlg
 