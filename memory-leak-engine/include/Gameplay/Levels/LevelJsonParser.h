#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace mlg {
    class LevelJsonParser {

    private:
        static void LoadLevelLayout(const json& jsonMapping);
    };
} //mlg