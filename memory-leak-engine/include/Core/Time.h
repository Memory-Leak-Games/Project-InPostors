#pragma once


namespace mlg {

    class Time {
    private:
        static Time* instance;

        double frameStart = 0.f;
        double lastFrameStart = 0.f;

        int fpsCap;
        int physicsTickRate;

    public:
        static void Initialize();
        static void Stop();

        static double GetSeconds();
        static float GetTrueDeltaSeconds();
        static float GetDeltaSeconds();

        static float GetFixedTimeStep();

        static void CapFPS();
        static void Sleep(double seconds);

        friend class Core;
    private:
        Time() = default;

        static void UpdateStartFrameTime();
    };

}// namespace mlg
