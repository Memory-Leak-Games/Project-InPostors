#pragma once

#include "KeyCodes.h"
#include <SDL2/SDL_gamecontroller.h>

namespace mlg {

    namespace Device {
        enum Type {
            Keyboard,
            Gamepad,
            Mouse
        };
    }

    class ActionMapping {
    private:
        Device::Type deviceType;

    public:
        ActionMapping() = delete;

        explicit ActionMapping(Device::Type deviceType)
            : deviceType(deviceType) {}

        [[nodiscard]] Device::Type GetDeviceType() const {
            return deviceType;
        }
    };

    class KeyboardActionMapping : public ActionMapping {
    private:
        Key::KeyCode keyCode;

    public:
        KeyboardActionMapping() = delete;

        explicit KeyboardActionMapping(Key::KeyCode keyCode)
            : ActionMapping(Device::Type::Keyboard), keyCode(keyCode) {}

        [[nodiscard]] Key::KeyCode GetKeyCode() const {
            return keyCode;
        }
    };

    class GamepadActionMapping : public ActionMapping {
    private:
        int deviceIndex;
        bool isAxis;
        bool isPositive = false;
        float deadZone = 0.f;
        float triggerZone = 0.f;
        Gamepad::GamepadButton buttonCode;
        Gamepad::GamepadAxis axisCode;

    public:
        GamepadActionMapping() = delete;

        explicit GamepadActionMapping(
                int deviceIndex,
                Gamepad::GamepadButton buttonCode)
            : ActionMapping(Device::Type::Gamepad),
              deviceIndex(deviceIndex),
              isAxis(false),
              buttonCode(buttonCode),
              axisCode(SDL_CONTROLLER_AXIS_INVALID) {}

        explicit GamepadActionMapping(
                int deviceIndex,
                float deadZone,
                float triggerZone,
                bool isPositive,
                Gamepad::GamepadAxis axisCode)
            : ActionMapping(Device::Type::Gamepad),
              deviceIndex(deviceIndex),
              isAxis(true),
              isPositive(isPositive),
              deadZone(deadZone),
              triggerZone(triggerZone),
              buttonCode(SDL_CONTROLLER_BUTTON_INVALID) {}

        [[nodiscard]] int GetDeviceIndex() const {
            return deviceIndex;
        }

        [[nodiscard]] bool IsAxis() const {
            return isAxis;
        }

        [[nodiscard]] float GetDeadZone() const {
            return deadZone;
        }

        [[nodiscard]] bool IsPositive() const {
            return isPositive;
        }

        [[nodiscard]] float GetTriggerZone() const {
            return triggerZone;
        }

        [[nodiscard]] Gamepad::GamepadButton GetButtonCode() const {
            return buttonCode;
        }

        [[nodiscard]] Gamepad::GamepadAxis GetAxisCode() const {
            return axisCode;
        }
    };

}// namespace mlg
