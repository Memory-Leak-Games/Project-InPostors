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

    constexpr float Time::GetFixedDeltaSeconds() {
        return fixedTimeStep;
    }

}// namespace mlg