#include "Core/HID/InputConfigParser.h"


namespace mlg {
    std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<InputAction>>>
    InputConfigParser::LoadConfig(const std::string& path) {
        auto inputActionsMap = std::make_unique<std::unordered_map<std::string, std::unique_ptr<InputAction>>>();

        std::ifstream configFile{path};
        json configJson = json::parse(configFile);

        for (const auto& jsonAction : configJson) {
            std::string actionName = jsonAction["name"].get<std::string>();
            auto newAction = std::make_unique<InputAction>(actionName);

            for (const auto& jsonMapping : jsonAction["mappings"]) {
                LoadActionMapping(jsonMapping, newAction.get());
            }

            inputActionsMap->insert({actionName, std::move(newAction)});
        }

        return std::move(inputActionsMap);
    }

    void InputConfigParser::LoadActionMapping(const json& jsonMapping, InputAction* inputAction) {
        const std::string deviceType = jsonMapping["deviceType"].get<std::string>();
        if (deviceType == "Keyboard") {
            LoadKeyboardMapping(jsonMapping, inputAction);
        } else if (deviceType == "Gamepad") {
            LoadGamepadMapping(jsonMapping, inputAction);
        }
    }

    void InputConfigParser::LoadKeyboardMapping(const json& jsonMapping, InputAction* inputAction) {
        Key::KeyCode keyCode = jsonMapping["code"].get<int>();
        auto keyboardMappingPtr = std::make_unique<KeyboardActionMapping>(keyCode);
        inputAction->AddMapping(std::move(keyboardMappingPtr));
    }

    void InputConfigParser::LoadGamepadMapping(const json& jsonMapping, InputAction* inputAction) {
        Gamepad::GamepadIndex gamepadIndex = jsonMapping["deviceIndex"].get<int>();
        bool isAxis = jsonMapping["isAxis"].get<bool>();
        uint16_t buttonOrAxis = jsonMapping["code"].get<int>();

        if (isAxis) {
            bool isPositive = jsonMapping["isPositive"].get<bool>();
            float triggerZone = jsonMapping["triggerZone"].get<float>();
            float deadZone = jsonMapping["deadZone"].get<float>();
            auto gamepadMappingPtr = std::make_unique<GamepadActionMapping>(gamepadIndex, deadZone,
                                                                            triggerZone, isPositive, buttonOrAxis);
            inputAction->AddMapping(std::move(gamepadMappingPtr));
        } else {
            auto gamepadMappingPtr = std::make_unique<GamepadActionMapping>(gamepadIndex, buttonOrAxis);
            inputAction->AddMapping(std::move(gamepadMappingPtr));
        }
    }
} // mlg