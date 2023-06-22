#pragma once

#include "InputAction.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include <unordered_set>
#include <vector>

#ifndef WIN32
#define MLG_INPUT_CONFIG_PATH "res/config/input.json"
#else
#define MLG_INPUT_CONFIG_PATH "res/config/input_windows.json"
#endif

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
        static void Initialize();
        static void Stop();

        static bool IsActionPressed(const std::string& actionName);
        static bool IsActionJustPressed(const std::string& actionName);
        static bool IsActionJustReleased(const std::string& actionName);

        static float GetActionStrength(const std::string& actionName);

        friend class Core;

    private:
        static void Update();

        void LoadActions();
        void FindGamepads();
        void ProcessSDLEvents();
        void HandleDeviceConenction(const SDL_Event& event);
        void HandleDeviceDisconnection(const SDL_Event& event);

        int FindSmallestValidGamepadIndex();
        bool IsGamepadPresent(const SDL_GameController* gamepad);
    };

}// namespace mlg
