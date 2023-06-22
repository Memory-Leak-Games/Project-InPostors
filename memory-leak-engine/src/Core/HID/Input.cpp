#include "Core/HID/Input.h"

#include "Core/HID/InputConfigParser.h"
#include "GLFW/glfw3.h"
#include "Macros.h"
#include "SDL2/SDL.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_joystick.h>
#include <spdlog/spdlog.h>

namespace mlg {
    Input* Input::instance;

    void Input::Initialize() {
        if (instance != nullptr)
            return;

        instance = new Input;

        bool error = SDL_Init(SDL_INIT_GAMECONTROLLER) < 0;

        if (error) {
            SPDLOG_ERROR("SDL_Init failed: {}", SDL_GetError());
            MLG_ASSERT(error);
        }

        instance->LoadActions();
        SPDLOG_INFO("INPUT: Loaded input config");
    }

    void Input::Stop() {
        SPDLOG_INFO("Stopping Input");

        delete instance;
        instance = nullptr;
    }

    void Input::Update() {
        ZoneScopedN("Input: Update");
        instance->ProcessSDLEvents();

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

    void Input::FindGamepads() {
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            if (!SDL_IsGameController(i))
                continue;

            if (gamepads.size() >= 2)
                break;

            SDL_GameController* controller = SDL_GameControllerOpen(i);

            if (IsGamepadPresent(controller))
                continue;

            int gamepadIndex = FindSmallestValidGamepadIndex();
            gamepads.push_back({gamepadIndex, controller});

            SPDLOG_INFO("Found gamepad: {} with id: {}",
                        SDL_GameControllerName(controller),
                        gamepadIndex);
        }
    }

    void Input::ProcessSDLEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            HandleDeviceConenction(event);
            HandleDeviceDisconnection(event);
        }
    }

    void Input::HandleDeviceConenction(const SDL_Event& event) {
        if (event.type != SDL_CONTROLLERDEVICEADDED || gamepads.size() > 2)
            return;

        FindGamepads();
    }

    void Input::HandleDeviceDisconnection(const SDL_Event& event) {
        if (event.type != SDL_CONTROLLERDEVICEREMOVED || gamepads.empty())
            return;

        int gamepadToRemove = -1;

        for (auto& gamepad : gamepads) {
            SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad.controller);
            SDL_JoystickID joystickId = SDL_JoystickInstanceID(joystick);

            if (event.cdevice.which != joystickId)
                continue;

            SPDLOG_INFO("Gamepad {}: {} disconnected", gamepad.index,
                        SDL_GameControllerName(gamepad.controller));

            SDL_GameControllerClose(gamepad.controller);
            gamepadToRemove = gamepad.index;
        }

        if (gamepadToRemove == -1)
            return;

        auto it = std::remove_if(
                gamepads.begin(), gamepads.end(),
                [gamepadToRemove](const Gamepad& gamepad) {
                    return gamepad.index == gamepadToRemove;
                });

        gamepads.erase(it, gamepads.end());
    }

    int Input::FindSmallestValidGamepadIndex() {
        std::vector<int> gamepadIndexes;

        for (auto& gamepad : gamepads) {
            gamepadIndexes.push_back(gamepad.index);
        }

        for (int i = 0; i < GAMEPADS_MAX_COUNT; ++i) {
            if (std::find(gamepadIndexes.begin(),
                          gamepadIndexes.end(), i) == gamepadIndexes.end()) {
                return i;
            }
        }

        return -1;
    }

    bool Input::IsGamepadPresent(const SDL_GameController* gamepad) {
        for (auto& gamepadInstance : gamepads) {
            if (gamepadInstance.controller == gamepad)
                return true;
        }

        return false;
    }


}// namespace mlg