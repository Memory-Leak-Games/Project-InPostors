#pragma once

#include <functional>
namespace mlg {

    class TimerManager {
        struct Timer {
            uint id;
            double startTimePoint;

            float timeToTrigger;
            bool repeat;

            std::function<void()> onTrigger;
        };

        static TimerManager* instance;
        TimerManager() = default;

        uint idCounter = 0;
        std::unordered_map<uint, Timer> timersMap;

    public:
        static void Initialize();
        static void Stop();

        static TimerManager* GetInstance();

        static void Update();

        uint SetTimer(float time, bool repeat, const std::function<void()>& function);
        void ClearTimer(uint id);

        bool IsTimerValid(uint id);
        float GetTimerRemainingTime(uint id);
        float GetTimerElapsedTime(uint id);
        float GetTimerRate(uint id);

    };

}// namespace mlg