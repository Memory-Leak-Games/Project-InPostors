#include "Core/HID/InputAction.h"

#include "Core/Window.h"
#include "GLFW/glfw3.h"

namespace mlg {
    InputAction::InputAction(std::string name)
        : name(std::move(name)) {}

    void InputAction::AddMapping(ActionMapping actionMapping) {
        actionMappings.push_back(actionMapping);
    }

    void InputAction::Update() {
        std::vector<InputAction::MappingValue> results;

        for (const ActionMapping& actionMapping : actionMappings) {
            Device::Type deviceType = actionMapping.GetDeviceType();

            if (deviceType == Device::Type::Keyboard) {
                results.push_back(ProcessKeyboardMapping(actionMapping));
            } else if (deviceType == Device::Type::Gamepad) {
                results.push_back(ProcessGamepadMapping(actionMapping));
            }
        }

        wasPressedLastFrame = isPressedThisFrame;

        axisValue = 0.f;
        for (const auto& result : results) {
            isPressedThisFrame |= result.digital;

            if (axisValue < result.analog)
                axisValue = result.analog;
        }
    }

    InputAction::MappingValue InputAction::ProcessKeyboardMapping(const ActionMapping& actionMapping) {
        auto& keyboardActionMapping = (KeyboardActionMapping&) actionMapping;
        MappingValue result{};

        result.digital |= glfwGetKey((GLFWwindow*) Window::GetInstance()->GetNativeWindowHandle(),
                                     keyboardActionMapping.GetKeyCode());
        result.analog = result.digital ? 1.f : 0.f;

        return result;
    }

    InputAction::MappingValue InputAction::ProcessGamepadMapping(const ActionMapping& actionMapping) {
        auto& gamepadActionMapping = (GamepadActionMapping&) actionMapping;
        MappingValue result{};

        GLFWgamepadstate state;
        glfwGetGamepadState(gamepadActionMapping.GetDeviceIndex(), &state);

        if (gamepadActionMapping.IsAxis()) {
            result.analog = state.axes[gamepadActionMapping.GetButtonCode()];
            result.digital |= result.analog >= gamepadActionMapping.GetTriggerZone();

            if (result.analog <= gamepadActionMapping.GetDeadZone())
                result.analog = 0.f;

        } else {
            result.digital |= state.buttons[gamepadActionMapping.GetButtonCode()];
            result.analog = result.digital ? 1.f : 0.f;
        }

        return result;
    }

    bool InputAction::IsPressed() const {
        return isPressedThisFrame;
    }

    bool InputAction::IsJustPressed() const {
        return isPressedThisFrame && !wasPressedLastFrame;
    }

    bool InputAction::IsJustReleased() const {
        return !isPressedThisFrame && wasPressedLastFrame;
    }

    float InputAction::GetStrength() const {
        return axisValue;
    }
}// namespace mlg