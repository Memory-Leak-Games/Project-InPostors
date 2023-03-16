#pragma once


namespace mlg {

    class Time {
    private:
        static std::chrono::time_point<std::chrono::system_clock> startTimePoint;
        static std::chrono::time_point<std::chrono::system_clock> frameStartTimePoint;
        static std::chrono::time_point<std::chrono::system_clock> lastFrameStartTimePoint;

    public:
        static void Initialize();

        static float GetSeconds();
        static float GetTrueDeltaSeconds();
        static float GetDeltaSeconds();
        [[deprecated("Unimplemented")]]static float GetFixedDeltaSeconds();

        friend class CoreEngine;

    private:
        static void SetStartTimePoint(const std::chrono::time_point<std::chrono::system_clock>& newStartTimePoint);
        static void SetFrameStartTimePoint(const std::chrono::time_point<std::chrono::system_clock>& newFrameStartTimePoint);
    };

}// namespace mlg
