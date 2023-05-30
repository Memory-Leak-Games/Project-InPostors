#include "Core/Time.h"

#include <GLFW/glfw3.h>

#include "Core/Settings/SettingsManager.h"

#include "Macros.h"

namespace mlg {
    Time* Time::instance;

    double Time::GetSeconds() {
        return glfwGetTime();
    }

    float Time::GetTrueDeltaSeconds() {
        return instance->frameStart - instance->lastFrameStart;
    }

    float Time::GetDeltaSeconds() {
        instance->lastFramesDeltaSeconds[instance->frameCount % AVERAGED_FRAMES] = GetTrueDeltaSeconds();

        float result = 0.f;
        for (double& deltaSeconds : instance->lastFramesDeltaSeconds) {
            if (deltaSeconds < 0.f)
                continue;

            result += (float) deltaSeconds;
        }

        result /= AVERAGED_FRAMES;

        // To prevent strange behaviour
        return std::min(result, 1.f/20.f);
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

    void Time::UpdateStartFrameTime() {
        instance->lastFrameStart = instance->frameStart;
        instance->frameStart = glfwGetTime();
        instance->frameCount++;
    }

    void Time::Sleep(double seconds) {
        double startSleepTime = GetSeconds();
        while (GetSeconds() - startSleepTime <= seconds) {
            // This is sleep method so I am empty
        }
    }

    void Time::CapFPS() {
        ZoneScoped;

        double minFramerate = 1 / (double) instance->fpsCap;
        while (GetSeconds() - instance->frameStart <= minFramerate) {
            // This is sleep method so I am empty
        }
    }

    float Time::GetFixedTimeStep() {
        return 1.f / (float) instance->physicsTickRate;
    }

    float Time::GetAITimeStep() {
        return 1.f / (float) instance->aiTickRate;
    }

}// namespace mlg