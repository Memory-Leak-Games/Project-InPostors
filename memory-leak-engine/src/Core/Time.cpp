#include "include/Core/Time.h"

namespace mlg {
    std::chrono::time_point<std::chrono::high_resolution_clock> Time::startTimePoint;
    std::chrono::time_point<std::chrono::high_resolution_clock> Time::frameStartTimePoint;
    std::chrono::time_point<std::chrono::high_resolution_clock> Time::lastFrameStartTimePoint;

    float Time::GetSeconds() {
        std::chrono::duration<float> timeFromStart = std::chrono::high_resolution_clock::now() - startTimePoint;
        return timeFromStart.count();
    }

    float Time::GetTrueDeltaSeconds() {
        std::chrono::duration<float> deltaSeconds = frameStartTimePoint - lastFrameStartTimePoint;
        return deltaSeconds.count();
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

    void Time::SetStartTimePoint(const std::chrono::time_point<std::chrono::system_clock>& newStartTimePoint) {
        Time::startTimePoint = newStartTimePoint;
    }

    void Time::SetFrameStartTimePoint(const std::chrono::time_point<std::chrono::system_clock>& newFrameStartTimePoint) {
        Time::lastFrameStartTimePoint = frameStartTimePoint;
        Time::frameStartTimePoint = newFrameStartTimePoint;
    }

}// namespace mlg