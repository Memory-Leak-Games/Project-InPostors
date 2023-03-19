#pragma once


namespace mlg {

    class Time {
    private:
        static Time* instance;

        float frameStart = 0.f;
        float lastFrameStart = 0.f;

    public:
        static void Initialize();
        static void Stop();

        static float GetSeconds();
        static float GetTrueDeltaSeconds();
        static float GetDeltaSeconds();
        [[deprecated("Unimplemented")]]static float GetFixedDeltaSeconds();

        friend class Core;

    private:
        Time() = default;

        static void UpdateStartFrameTime();
    };

}// namespace mlg
