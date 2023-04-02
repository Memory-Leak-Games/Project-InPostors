#pragma once


namespace mlg {

    class Time {
    private:
        static Time* instance;

        constexpr const static float fixedTimeStep = 1.f / 30.f;

        double frameStart = 0.f;
        double lastFrameStart = 0.f;

    public:
        static void Initialize();

        static void Stop();

        static double GetSeconds();

        static float GetTrueDeltaSeconds();

        static float GetDeltaSeconds();

        constexpr static float GetFixedTimeStep() {
            return fixedTimeStep;
        };

        friend class Core;

    private:
        Time() = default;

        static void UpdateStartFrameTime();
    };

}// namespace mlg
