#include "include/Core/Time.h"
#include "GLFW/glfw3.h"

namespace mlg {
    Time* Time::instance;

    float Time::GetSeconds() {
        return (float) glfwGetTime();
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

    float Time::GetFixedDeltaSeconds() {
        return 1. / 30.;
    }

    void Time::Initialize() {
        instance = new Time;
        glfwSetTime(0.);
    }

    void Time::Stop() {
    }

    void Time::UpdateStartFrameTime() {
        instance->lastFrameStart = instance->frameStart;
        instance->frameStart = (float) glfwGetTime();
    }

}// namespace mlg