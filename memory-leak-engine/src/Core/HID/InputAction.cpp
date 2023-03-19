#include "Core/HID/InputAction.h"

#include "Core/Window.h"
#include "GLFW/glfw3.h"

namespace mlg {
    InputAction::InputAction(std::string name)
        : name(std::move(name)) {}

    void InputAction::AddMapping(std::unique_ptr<ActionMapping> actionMapping) {
        actionMappingsVector.push_back(std::move(actionMapping));
    }

    void InputAction::Update() {
        std::vector<InputAction::MappingValue> results;

        for (const std::unique_ptr<ActionMapping>& actionMapping : actionMappingsVector) {
            auto test = actionMapping.get();

            Device::Type deviceType = actionMapping->GetDeviceType();

            if (deviceType == Device::Type::Keyboard) {
                results.push_back(ProcessKeyboardMapping(actionMapping.get()));
            } else if (deviceType == Device::Type::Gamepad) {
                results.push_back(ProcessGamepadMapping(actionMapping.get()));
            }
        }

        wasPressedLastFrame = isPressedThisFrame;

        axisValue = 0.f;
        isPressedThisFrame = false;
        for (const auto& result : results) {
            isPressedThisFrame |= result.digital;

            if (axisValue < result.analog)
                axisValue = result.analog;
        }
    }

    InputAction::MappingValue InputAction::ProcessKeyboardMapping(const ActionMapping* actionMapping) {
        auto keyboardActionMapping = (KeyboardActionMapping*) actionMapping;
        MappingValue result{};

        result.digital = glfwGetKey((GLFWwindow*) Window::GetInstance()->GetNativeWindowHandle(),
                                     keyboardActionMapping->GetKeyCode());
        result.analog = result.digital ? 1.f : 0.f;

        return result;
    }

    InputAction::MappingValue InputAction::ProcessGamepadMapping(const ActionMapping* actionMapping) {
        auto gamepadActionMapping = (GamepadActionMapping*) actionMapping;
        MappingValue result{};

        Gamepad::GamepadIndex gamepadIndex = gamepadActionMapping->GetDeviceIndex();

        if (!glfwJoystickPresent(gamepadActionMapping->GetDeviceIndex()))
            return result;

        int axesCount = Gamepad::AxesCount;
        const float* axes = glfwGetJoystickAxes(gamepadIndex, &axesCount);
        int buttonsCount = Gamepad::ButtonsCount;
        const uint8_t* buttons = glfwGetJoystickButtons(gamepadIndex, &buttonsCount);

        if (gamepadActionMapping->IsAxis()) {
            float value = axes[gamepadActionMapping->GetButtonCode()];

            if (gamepadActionMapping->IsPositive() && value < 0.f)
                value = 0.f;

            if (!gamepadActionMapping->IsPositive() && value > 0.f)
                value = 0.f;

            result.analog = std::abs(value);
            result.digital = result.analog >= gamepadActionMapping->GetTriggerZone();

            if (result.analog <= gamepadActionMapping->GetDeadZone())
                result.analog = 0.f;

        } else {
            result.digital = buttons[gamepadActionMapping->GetButtonCode()];
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