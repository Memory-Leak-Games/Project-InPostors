#pragma once

#define AVERAGED_FRAMES 5
#define MINIMAL_DELTA_SECONDS (1.f / 20.f)

namespace mlg {

    class Time {
    private:
        static Time* instance;

        double frameStart = 0.f;
        double lastFrameStart = 0.f;

        uint64_t frameCount;

        std::array<double, AVERAGED_FRAMES> lastFramesDeltaSeconds;

        int fpsCap;
        int physicsTickRate;
        int aiTickRate;

        bool gamePaused = false;
        double timePaused = 0.f;

    public:
        static void Initialize();
        static void Stop();

        static double GetTrueSeconds();
        static double GetSeconds();
        static float GetTrueDeltaSeconds();
        static float GetDeltaSeconds();

        static float GetFixedTimeStep();
        static float GetAITimeStep();

        static bool IsGamePaused();
        static void PauseGame(bool gamePaused);

        static void CapFPS();
        static void Sleep(double seconds);

        friend class Core;
    private:
        Time() = default;

        static void Update();
    };

}// namespace mlg
