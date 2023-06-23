#pragma once

#include "InputAction.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include <eventpp/callbacklist.h>
#include <eventpp/eventqueue.h>
#include <unordered_set>
#include <vector>

#define MLG_INPUT_CONFIG_PATH "res/config/input.json"

#define GAMEPADS_MAX_COUNT 2

namespace mlg {

    class Input {
    private:
        static Input* instance;

        struct Gamepad {
            int index;
            SDL_GameController* controller;
        };

        std::vector<Gamepad> gamepads;

        // Action Name, ActionPtr
        std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<InputAction>>> inputActionsMap;

    public:
        eventpp::CallbackList<void(int)> OnDeviceConnected;
        eventpp::CallbackList<void(int)> OnDeviceDisconnected;

        static void Initialize();
        static void Stop();
        static Input* Get();

        static bool IsActionPressed(const std::string& actionName);
        static bool IsActionJustPressed(const std::string& actionName);
        static bool IsActionJustReleased(const std::string& actionName);

        static float GetActionStrength(const std::string& actionName);

        static void SetVibration(
                int index, float lowStrength,
                float hightStrength, float duration);

        bool IsGamepadPresent(const SDL_GameController* gamepad);
        SDL_GameController* GetGamepad(int index);

        static void DebugInput();
        static void Update();

    private:
        void LoadActions();
        void FindGamepads();
        void ProcessSDLEvents();
        void HandleDeviceConenction(const SDL_Event& event);
        void HandleDeviceDisconnection(const SDL_Event& event);

        int FindSmallestValidGamepadIndex();
    };

}// namespace mlg
