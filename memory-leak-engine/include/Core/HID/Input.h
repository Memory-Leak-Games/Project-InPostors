#pragma once

#include "InputAction.h"

#define MLG_INPUT_CONFIG_PATH "res/config/input.json"

namespace mlg {

    class Input {
    private:
        static Input* instance;

        // Action Name, ActionPtr
        std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<InputAction>>> inputActionsMap;

    public:
        static void Initialize();
        static void Stop();

        static bool IsActionPressed(const std::string& actionName);
        static bool IsActionJustPressed(const std::string& actionName);
        static bool IsActionJustReleased(const std::string& actionName);

        static float GetActionStrength(const std::string& actionName);

        friend class Core;

    private:
        static void Update();

        void LoadActions();
    };

}// namespace mlg
