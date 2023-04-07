#include "include/Core/Time.h"
#include "GLFW/glfw3.h"

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
        glfwSetTime(0.);

        SPDLOG_INFO("Initializing Time");
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
        while (GetSeconds() - instance->frameStart <= 1 / (double) instance->maxFPS) {
            // This is sleep method so I am empty
        }
    }

}// namespace mlg