#pragma once


namespace mlg {

    class Time {
    private:
        static std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<float>> startTimePoint;
        static std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<float>> frameStartTimePoint;
        static std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<float>> lastFrameEndTimePoint;

    public:
        static float GetSeconds();
        static float GetTrueDeltaSeconds();
        static float GetDeltaSeconds();
        [[deprecated("Unimplemented")]]static float GetFixedDeltaSeconds();

        friend class CoreEngine;

    private:
        static void SetStartTimePoint(const std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<float>>& newStartTimePoint);
        static void SetFrameStartTimePoint(const std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<float>>& newFrameStartTimePoint);
        static void SetLastFrameEndTimePoint(const std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<float>>& newLastFrameEndTimePoint);
    };

}// namespace mlg
