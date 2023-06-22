#include "Core/HID/InputAction.h"

#include "Core/HID/Input.h"
#include "Core/HID/KeyCodes.h"
#include "Core/Window.h"

#include <GLFW/glfw3.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

namespace mlg {
    InputAction::InputAction(std::string name)
        : name(std::move(name)) {}

    void InputAction::AddMapping(std::unique_ptr<ActionMapping> actionMapping) {
        actionMappingsVector.push_back(std::move(actionMapping));
    }

    void InputAction::Update() {
        std::vector<InputAction::MappingValue> results;

        for (const std::unique_ptr<ActionMapping>& actionMapping : actionMappingsVector) {
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

        if (Window::Get() == nullptr)
            return result;

        GLFWwindow* window = (GLFWwindow*) Window::Get()->GetNativeWindowHandle();
        result.digital = glfwGetKey(window, keyboardActionMapping->GetKeyCode());
        result.analog = result.digital ? 1.f : 0.f;

        return result;
    }

    InputAction::MappingValue InputAction::ProcessGamepadMapping(const ActionMapping* actionMapping) {
        auto gamepadActionMapping = (GamepadActionMapping*) actionMapping;
        MappingValue result{false, 0.f};

        int gamepadIndex = gamepadActionMapping->GetDeviceIndex();
        SDL_GameController* gamepad = Input::Get()->GetGamepad(gamepadIndex);

        if (!gamepad)
            return result;

        if (gamepadActionMapping->IsAxis()) {
            float value = SDL_GameControllerGetAxis(
                    gamepad, gamepadActionMapping->GetAxisCode());
            value /= 32767.f; // 2^15 - 1

            if (gamepadActionMapping->IsPositive() && value < 0.f)
                value = 0.f;

            if (!gamepadActionMapping->IsPositive() && value > 0.f)
                value = 0.f;

            result.analog = std::abs(value);
            result.digital = result.analog >= gamepadActionMapping->GetTriggerZone();

            if (result.analog <= gamepadActionMapping->GetDeadZone())
                result.analog = 0.f;

        } else {
            result.digital = SDL_GameControllerGetButton(
                    gamepad, gamepadActionMapping->GetButtonCode());
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