#pragma once

#include "InputAction.h"

namespace mlg {

    class Input {
    private:
        static Input* instance;

        // Action Name, Action
        std::unordered_map<std::string, InputAction> inputActionsMap;

    public:
        static void Init();
        static void Stop();

        static bool IsActionPressed(const std::string& actionName);
        static bool IsActionJustPressed(const std::string& actionName);
        static bool IsActionJustReleased(const std::string& actionName);

        static float GetActionStrength(const std::string& actionName);

        friend class Core;

    private:
        static void Update();

        void LoadMappings();
    };

}// namespace mlg
