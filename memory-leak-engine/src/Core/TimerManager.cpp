#include "Core/TimerManager.h"
#include <algorithm>
#include <cstddef>
#include <spdlog/spdlog.h>
#include <vector>

#include "Core/Time.h"

mlg::TimerManager* mlg::TimerManager::instance;

void mlg::TimerManager::Initialize() {
    if (instance != nullptr)
        return;

    SPDLOG_INFO("Initializing TimerManager");
    instance = new TimerManager();
}

void mlg::TimerManager::Stop() {
    if (instance == nullptr)
        return;

    SPDLOG_INFO("Stopping TimerManager");
    delete instance;

    instance = nullptr;
}

mlg::TimerManager* mlg::TimerManager::GetInstance() {
    return instance;
}

void mlg::TimerManager::Update() {
    const double currentTimePoint = Time::GetSeconds();

    std::vector<uint> timersToRemove;

    for (auto& [id, timer] : instance->timersMap) {
        const float elapsedTime = instance->GetTimerElapsedTime(id);
        if (elapsedTime < timer.timeToTrigger) 
            continue;

        timer.onTrigger();

        if (timer.repeat) {
            timer.startTimePoint = currentTimePoint;
        } else {
            timersToRemove.push_back(id);
        }
    }

    for (const auto& id: timersToRemove) {
        instance->ClearTimer(id);
    }
}

uint mlg::TimerManager::SetTimer(float time, bool repeat, const std::function<void()>& function) {
    Timer newTimer;
    newTimer.id = idCounter;
    newTimer.timeToTrigger = time;
    newTimer.repeat = repeat;
    newTimer.onTrigger = function;

    newTimer.startTimePoint = Time::GetSeconds();

    timersMap.insert({idCounter, newTimer});

    idCounter++;

    return newTimer.id;
}

void mlg::TimerManager::ClearTimer(uint id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end()) {
        SPDLOG_WARN("Timer with id: {} not found.", id);
        return;
    }

    timersMap.erase(foundIterator);
}

bool mlg::TimerManager::IsTimerValid(uint id) {
    return timersMap.find(id) != timersMap.end();
}

float mlg::TimerManager::GetTimerRemainingTime(uint id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end())
        return -1.f;

    const auto currentTimePoint = (float) Time::GetSeconds();
    const auto elapsedTime = (float) (currentTimePoint - foundIterator->second.startTimePoint);
    return foundIterator->second.timeToTrigger - elapsedTime;
}

float mlg::TimerManager::GetTimerElapsedTime(uint id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end())
        return -1.f;

    const auto currentTimePoint = (float) Time::GetSeconds();
    return (float) (currentTimePoint - foundIterator->second.startTimePoint);
}

float mlg::TimerManager::GetTimerRate(uint id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end())
        return -1.f;

    return foundIterator->second.timeToTrigger;
}