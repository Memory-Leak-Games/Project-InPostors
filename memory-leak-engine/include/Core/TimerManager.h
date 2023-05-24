#pragma once

#include <cstdint>
#include <functional>
namespace mlg {

    class TimerManager {
        struct Timer {
            uint32_t id;
            double startTimePoint;

            float timeToTrigger;
            bool repeat;

            std::function<void()> onTrigger;
        };

        static TimerManager* instance;
        TimerManager() = default;

        uint32_t idCounter = 0;
        std::unordered_map<unsigned int, Timer> timersMap;

    public:
        static void Initialize();
        static void Stop();

        static TimerManager* GetInstance();

        static void Update();

        uint32_t SetTimer(float time, bool repeat, const std::function<void()>& function);
        void ClearTimer(uint32_t id);

        bool IsTimerValid(uint32_t id);
        float GetTimerRemainingTime(uint32_t id);
        float GetTimerElapsedTime(uint32_t id);
        float GetTimerRate(uint32_t id);

    };

}// namespace mlg