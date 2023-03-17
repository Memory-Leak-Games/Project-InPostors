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
        float deadZone = 0.f;
        float triggerZone = 0.f;
        Gamepad::GamepadCode buttonCode;

    public:
        explicit GamepadActionMapping(Gamepad::GamepadIndex deviceIndex, Gamepad::GamepadCode buttonCode)
            : ActionMapping(Device::Type::Gamepad), deviceIndex(deviceIndex), isAxis(false), buttonCode(buttonCode) {}

        explicit GamepadActionMapping(Gamepad::GamepadIndex deviceIndex, bool isAxis, float deadZone, float triggerZone, Gamepad::GamepadCode buttonCode)
            : ActionMapping(Device::Type::Gamepad), deviceIndex(deviceIndex), isAxis(isAxis), deadZone(deadZone),
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

        [[nodiscard]] float GetTriggerZone() const {
            return triggerZone;
        }

        [[nodiscard]] Gamepad::GamepadCode GetButtonCode() const {
            return buttonCode;
        }
    };

}// namespace mlg
