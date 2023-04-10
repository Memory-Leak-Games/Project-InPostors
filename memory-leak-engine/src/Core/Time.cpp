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
        float result = GetTrueDeltaSeconds();

        // To prevent strange behaviour
        if (result > 1. / 15.)
            result = 1. / 15.;

        return result;
    }

    void Time::Initialize() {
        instance = new Time;

        SPDLOG_INFO("Initializing Time");
        glfwSetTime(0.);

        instance->fpsCap = SettingsManager::Get<int>(SettingsType::Engine, "fpsCAP");
        instance->physicsTickRate = SettingsManager::Get<int>(SettingsType::Engine, "physicsTickRate");
    }

    void Time::Stop() {
        delete instance;
        instance = nullptr;

        SPDLOG_INFO("Stopping Time");
    }

    void Time::UpdateStartFrameTime() {
        instance->lastFrameStart = instance->frameStart;
        instance->frameStart = glfwGetTime();
    }

    void Time::Sleep(double seconds) {
        double startSleepTime = GetSeconds();
        while (GetSeconds() - startSleepTime <= seconds) {
            // This is sleep method so I am empty
        }
    }

    void Time::CapFPS() {
        double minFramerate = 1 / (double) instance->fpsCap;
        while (GetSeconds() - instance->frameStart <= minFramerate) {
            // This is sleep method so I am empty
        }
    }

    float Time::GetFixedTimeStep() {
        return 1.f / (float) instance->physicsTickRate;
    }

}// namespace mlg