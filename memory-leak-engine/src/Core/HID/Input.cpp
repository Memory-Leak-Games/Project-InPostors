#include "Core/HID/Input.h"

#include "Macros.h"
#include "Core/HID/InputConfigParser.h"
#include "GLFW/glfw3.h"

namespace mlg {
    Input* Input::instance;

    void Input::Initialize() {
        if (instance != nullptr)
            return;

        instance = new Input;

        instance->LoadActions();
        SPDLOG_INFO("INPUT: Loaded input config");
    }

    void Input::Stop() {
        SPDLOG_INFO("Stopping Input");

        delete instance;
        instance = nullptr;
    }

    void Input::Update() {
#ifdef DEBUG
        // to have instance in debbuger:
        auto localInstance  = instance;
#endif
        for (auto& [key, value] : *instance->inputActionsMap) {
            value->Update();
        }
    }

    bool Input::IsActionPressed(const std::string& actionName) {
        auto iterator = instance->inputActionsMap->find(actionName);

        if (iterator == instance->inputActionsMap->end()) {
            SPDLOG_ERROR("Action {} doesn't exist", actionName);
            return false;
        }

        return iterator->second->IsPressed();
    }

    bool Input::IsActionJustPressed(const std::string& actionName) {
        auto iterator = instance->inputActionsMap->find(actionName);

        if (iterator == instance->inputActionsMap->end()) {
            SPDLOG_ERROR("Action {} doesn't exist", actionName);
            return false;
        }

        return iterator->second->IsJustPressed();
    }

    bool Input::IsActionJustReleased(const std::string& actionName) {
        auto iterator = instance->inputActionsMap->find(actionName);

        if (iterator == instance->inputActionsMap->end()) {
            SPDLOG_ERROR("Action {} doesn't exist", actionName);
            return false;
        }

        return iterator->second->IsJustReleased();
    }

    float Input::GetActionStrength(const std::string& actionName) {
        auto iterator = instance->inputActionsMap->find(actionName);

        if (iterator == instance->inputActionsMap->end()) {
            SPDLOG_ERROR("Action {} doesn't exist", actionName);
            return 0.f;
        }

        return iterator->second->GetStrength();
    }

    void Input::LoadActions() {
        inputActionsMap = std::move(InputConfigParser::LoadConfig(MLG_INPUT_CONFIG_PATH));
    }
}// namespace mlg