#pragma once

#include "InputAction.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace mlg {

    class InputConfigParser {
    public:
        static std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<InputAction>>> LoadConfig(const std::string& path);

    private:
        static void LoadActionMapping(const json& jsonMapping, InputAction* inputAction);
        static void LoadGamepadMapping(const json& jsonMapping, InputAction* inputAction);
        static void LoadKeyboardMapping(const json& jsonMapping, InputAction* inputAction);
    };

} // mlg
