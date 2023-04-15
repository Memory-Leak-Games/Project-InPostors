#pragma once

#include <fstream>
#include "nlohmann/json.hpp"

namespace mlg {

    using json = nlohmann::json;

    class LevelLayout {
    public:
        static void LoadLevelJson(const std::string& path);
    private:
        static void LoadLayout(const json& jsonMapping);
    };

} // mlg
