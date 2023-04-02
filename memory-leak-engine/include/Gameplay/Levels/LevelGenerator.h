#pragma once
#include "pch.h"
#include "Rendering/Model.h"

namespace mlg {

    class LevelGenerator {
    public:
        static bool GenerateLevel();
        static bool GenerateTestLevel();

    private:
        static bool PutObject(const std::string& modelPath, float posX, float posY, float rotation = 0.0f);
        static std::string Hash(const std::string& modelPath, float posX, float posY);
    };

} //mlg
 