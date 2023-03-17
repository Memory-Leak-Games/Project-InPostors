#pragma once

#include "ActionMapping.h"

namespace mlg {

    class InputAction {
    private:
        struct MappingValue {
            bool digital;
            float analog;
        };

        std::string name;
        std::vector<ActionMapping> actionMappings;

        bool wasPressedLastFrame = false;
        bool isPressedThisFrame = false;
        float axisValue = 0;

    public:
        explicit InputAction(std::string  name);
        void AddMapping(ActionMapping actionMapping);

        void Update();

        [[nodiscard]] bool IsPressed() const;
        [[nodiscard]] bool IsJustPressed() const;
        [[nodiscard]] bool IsJustReleased() const;

        [[nodiscard]] float GetStrength() const;

    private:
        static MappingValue ProcessKeyboardMapping(const ActionMapping& actionMapping);
        static MappingValue ProcessGamepadMapping(const ActionMapping& actionMapping);
    };

}// namespace mlg
