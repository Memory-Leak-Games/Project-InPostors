#include "Core/Time.h"

#include <GLFW/glfw3.h>

#include "Core/Settings/SettingsManager.h"

#include "Macros.h"


namespace mlg {
    Time* Time::instance;

    double Time::GetTrueSeconds() {
        return glfwGetTime();
    }

    double Time::GetSeconds() {
        return GetTrueSeconds() - instance->timePaused;
    }

    float Time::GetTrueDeltaSeconds() {
        return instance->frameStart - instance->lastFrameStart;
    }

    float Time::GetDeltaSeconds() {
        if (instance->gamePaused)
            return 0.f;

        instance->lastFramesDeltaSeconds[instance->frameCount % AVERAGED_FRAMES] = GetTrueDeltaSeconds();

        float result = 0.f;
        int averagedFrames = 0;
        for (double& deltaSeconds : instance->lastFramesDeltaSeconds) {
            if (deltaSeconds <= 0.f)
                continue;

            result += (float) deltaSeconds;
            averagedFrames++;
        }

        result /= (float) averagedFrames;

        // To prevent strange behaviour
        return std::min(result, MINIMAL_DELTA_SECONDS);
    }

    void Time::Initialize() {
        instance = new Time;

        SPDLOG_INFO("Initializing Time");
        glfwSetTime(0.);

        instance->fpsCap = SettingsManager::Get<int>(SettingsType::Engine, "fpsCAP");
        instance->physicsTickRate = SettingsManager::Get<int>(SettingsType::Engine, "physicsTickRate");
        instance->aiTickRate = SettingsManager::Get<int>(SettingsType::Engine, "aiTickRate");

        instance->frameCount = 0;
        for (double& deltaTime : instance->lastFramesDeltaSeconds) {
            deltaTime = -1.;
        }
    }

    void Time::Stop() {
        delete instance;
        instance = nullptr;

        SPDLOG_INFO("Stopping Time");
    }

    void Time::Update() {
        instance->lastFrameStart = instance->frameStart;
        instance->frameStart = glfwGetTime();
        instance->frameCount++;

        if (instance->gamePaused)
            instance->timePaused += GetTrueDeltaSeconds();
    }

    void Time::Sleep(double seconds) {
        double startSleepTime = GetTrueSeconds();
        while (GetSeconds() - startSleepTime <= seconds) {
            // This is sleep method so I am empty
        }
    }

    void Time::CapFPS() {
        ZoneScoped;

        double minFramerate = 1 / (double) instance->fpsCap;
        while (GetTrueSeconds() - instance->frameStart <= minFramerate) {
            // This is sleep method so I am empty
        }
    }

    float Time::GetFixedTimeStep() {
        return 1.f / (float) instance->physicsTickRate;
    }

    float Time::GetAITimeStep() {
        return 1.f / (float) instance->aiTickRate;
    }

    bool Time::IsGamePaused() {
        return instance->gamePaused;
    }

    void Time::PauseGame(bool gamePaused) {
        instance->gamePaused = gamePaused;
    }

}// namespace mlg