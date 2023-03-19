#pragma once

#include "KeyCodes.h"

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
        Gamepad::GamepadIndex deviceIndex;
        bool isAxis;
        bool isPositive = false;
        float deadZone = 0.f;
        float triggerZone = 0.f;
        Gamepad::GamepadCode buttonCode;

    public:
        GamepadActionMapping() = delete;

        explicit GamepadActionMapping(Gamepad::GamepadIndex deviceIndex, Gamepad::GamepadCode buttonCode)
            : ActionMapping(Device::Type::Gamepad), deviceIndex(deviceIndex), isAxis(false), buttonCode(buttonCode) {}

        explicit GamepadActionMapping(Gamepad::GamepadIndex deviceIndex, float deadZone, float triggerZone, bool isPositive, Gamepad::GamepadCode buttonCode)
            : ActionMapping(Device::Type::Gamepad), deviceIndex(deviceIndex), isAxis(true), isPositive(isPositive), deadZone(deadZone),
              triggerZone(triggerZone), buttonCode(buttonCode) {}

        [[nodiscard]] Gamepad::GamepadIndex GetDeviceIndex() const {
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

        [[nodiscard]] Gamepad::GamepadCode GetButtonCode() const {
            return buttonCode;
        }
    };

}// namespace mlg
