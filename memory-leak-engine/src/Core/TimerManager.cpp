#include "Core/TimerManager.h"

#include "Core/Time.h"
#include <cstddef>

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

mlg::TimerManager* mlg::TimerManager::Get() {
    return instance;
}

void mlg::TimerManager::Update() {
    const double currentTimePoint = Time::GetSeconds();

    std::vector<size_t> timersToRemove;

    for (auto& [id, timer] : instance->timersMap) {
        const float elapsedTime = instance->GetTimerElapsedTime(id);
        if (elapsedTime < timer.timeToTrigger)
            continue;

        if (timer.repeat) {
            timer.startTimePoint = currentTimePoint;
        } else {
            timer.valid = false;
            timersToRemove.push_back(id);
        }

        timer.onTrigger();
    }

    for (const auto& id : timersToRemove) {
        instance->ClearTimer(id);
    }
}

size_t mlg::TimerManager::SetTimer(float time, bool repeat, const std::function<void()>& function) {
    Timer newTimer;
    newTimer.id = idCounter;
    newTimer.timeToTrigger = time;
    newTimer.repeat = repeat;
    newTimer.onTrigger = function;
    newTimer.valid = true;

    newTimer.startTimePoint = Time::GetSeconds();

    timersMap.insert({idCounter, newTimer});

    idCounter++;

    return newTimer.id;
}

void mlg::TimerManager::ClearTimer(size_t id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end()) {
        SPDLOG_WARN("Timer with id: {} not found.", id);
        return;
    }

    timersMap.erase(foundIterator);
}

bool mlg::TimerManager::IsTimerValid(size_t id) {
    return timersMap.find(id) != timersMap.end() && timersMap.at(id).valid;
}

float mlg::TimerManager::GetTimerRemainingTime(size_t id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end())
        return -1.f;

    const auto currentTimePoint = (float) Time::GetSeconds();
    const auto elapsedTime = (float) (currentTimePoint - foundIterator->second.startTimePoint);
    return foundIterator->second.timeToTrigger - elapsedTime;
}

float mlg::TimerManager::GetTimerElapsedTime(size_t id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end())
        return -1.f;

    const auto currentTimePoint = (float) Time::GetSeconds();
    return (float) (currentTimePoint - foundIterator->second.startTimePoint);
}

float mlg::TimerManager::GetTimerRate(size_t id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end())
        return -1.f;

    return foundIterator->second.timeToTrigger;
}

float mlg::TimerManager::GetPercentage(size_t id) {
    auto foundIterator = timersMap.find(id);
    if (foundIterator == timersMap.end())
        return -1.f;

    const auto currentTimePoint = (float) Time::GetSeconds();
    double startTimePoint = foundIterator->second.startTimePoint;
    float elapsedTime = (float) (currentTimePoint - startTimePoint);

    return (float) elapsedTime / foundIterator->second.timeToTrigger;
}
