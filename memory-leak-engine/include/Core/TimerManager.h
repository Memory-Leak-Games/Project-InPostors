#pragma once

#include <cstdint>
#include <functional>
namespace mlg {

    class TimerManager {
        struct Timer {
            size_t id;
            double startTimePoint;

            float timeToTrigger;
            bool repeat;

            std::function<void()> onTrigger;

            bool valid;
        };

        static TimerManager* instance;
        TimerManager() = default;

        size_t idCounter = 1;
        std::unordered_map<size_t, Timer> timersMap;

    public:
        static void Initialize();
        static void Stop();

        static TimerManager* Get();

        static void Update();

        size_t SetTimer(float time, bool repeat, const std::function<void()>& function);
        void ClearTimer(size_t id);

        bool IsTimerValid(size_t id);
        float GetTimerRemainingTime(size_t id);
        float GetTimerElapsedTime(size_t id);
        float GetTimerRate(size_t id);

    };

}// namespace mlg