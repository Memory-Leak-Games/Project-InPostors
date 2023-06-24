#include "Core/HID/Input.h"

#include "Core/HID/InputConfigParser.h"
#include "Macros.h"
#include "SDL_stdinc.h"

#include <GLFW/glfw3.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_joystick.h>
#include <array>
#include <cstdint>

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

    Input* Input::Get() {
        return instance;
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

    void Input::SetVibration(
            int index, float lowStrength,
            float hightStrength, float duration) {
        SDL_GameController* gamepad = instance->GetGamepad(index);
        if (gamepad == nullptr)
            return;

        SDL_GameControllerRumble(
                gamepad,
                (uint16_t) (lowStrength * 0xFFFF),
                (uint16_t) (hightStrength * 0xFFFF),
                (uint32_t) (duration * 1000.f));
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

            OnDeviceConnected(gamepadIndex);

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
            
            OnDeviceDisconnected(gamepad.index);
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

    SDL_GameController* Input::GetGamepad(int index) {
        for (auto& gamepadInstance : gamepads) {
            if (gamepadInstance.index == index)
                return gamepadInstance.controller;
        }

        return nullptr;
    }

    void Input::DebugInput() {
        auto buttons = std::array{
                SDL_CONTROLLER_BUTTON_A,
                SDL_CONTROLLER_BUTTON_B,
                SDL_CONTROLLER_BUTTON_X,
                SDL_CONTROLLER_BUTTON_Y,
                SDL_CONTROLLER_BUTTON_BACK,
                SDL_CONTROLLER_BUTTON_GUIDE,
                SDL_CONTROLLER_BUTTON_START,
                SDL_CONTROLLER_BUTTON_LEFTSTICK,
                SDL_CONTROLLER_BUTTON_RIGHTSTICK,
                SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
                SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
                SDL_CONTROLLER_BUTTON_DPAD_UP,
                SDL_CONTROLLER_BUTTON_DPAD_DOWN,
                SDL_CONTROLLER_BUTTON_DPAD_LEFT,
                SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
        };

        auto axises = std::array{
                SDL_CONTROLLER_AXIS_LEFTX,
                SDL_CONTROLLER_AXIS_LEFTY,
                SDL_CONTROLLER_AXIS_RIGHTX,
                SDL_CONTROLLER_AXIS_RIGHTY,
                SDL_CONTROLLER_AXIS_TRIGGERLEFT,
                SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
        };

        static std::array<float, 6> axisesStates;

        for (auto& gamepad : instance->gamepads) {
            for (auto& button : buttons) {
                if (SDL_GameControllerGetButton(
                            gamepad.controller, button)) {

                    SPDLOG_INFO("Gamepad {}: {}, id: {} pressed", gamepad.index,
                                SDL_GameControllerGetStringForButton(button),
                                button);
                }
            }
            for (auto& axis : axises) {
                int16_t axisValue = SDL_GameControllerGetAxis(
                        gamepad.controller, axis);
                float value = (float) axisValue / 32767.f;

                if (value != axisesStates[axis]) {
                    SPDLOG_INFO("Gamepad {}: {}, id: {} value: {}", gamepad.index,
                                SDL_GameControllerGetStringForAxis(axis),
                                axis, value);
                }

                axisesStates[axis] = value;
            }
        }
    }


}// namespace mlg