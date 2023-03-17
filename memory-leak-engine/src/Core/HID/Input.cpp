#include "Core/HID/Input.h"

#include "Macros.h"

namespace mlg {
    Input* Input::instance;

    void Input::Init() {
    }

    bool Input::IsActionPressed(const std::string& actionName) {
        auto iterator = instance->inputActionsMap.find(actionName);

        if (iterator == instance->inputActionsMap.end()) {
            SPDLOG_ERROR("Action {} doesn't exist", actionName);
            return false;
        }

        return iterator->second.IsPressed();
    }

    bool Input::IsActionJustPressed(const std::string& actionName) {
        auto iterator = instance->inputActionsMap.find(actionName);

        if (iterator == instance->inputActionsMap.end()) {
            SPDLOG_ERROR("Action {} doesn't exist", actionName);
            return false;
        }

        return iterator->second.IsJustPressed();
    }

    bool Input::IsActionJustReleased(const std::string& actionName) {
        auto iterator = instance->inputActionsMap.find(actionName);

        if (iterator == instance->inputActionsMap.end()) {
            SPDLOG_ERROR("Action {} doesn't exist", actionName);
            return false;
        }

        return iterator->second.IsJustReleased();
    }

    float Input::GetActionStrength(const std::string& actionName) {
        auto iterator = instance->inputActionsMap.find(actionName);

        if (iterator == instance->inputActionsMap.end()) {
            SPDLOG_ERROR("Action {} doesn't exist", actionName);
            return 0.f;
        }

        return iterator->second.GetStrength();
    }
}// namespace mlg