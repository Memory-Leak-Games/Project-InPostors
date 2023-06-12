#pragma once

#include "Gameplay/Entity.h"
#include "Audio/Assets/AudioAsset.h"
#include <cstddef>
#include <eventpp/callbacklist.h>

namespace mlg {
    class Label;
}// namespace mlg

class StartLevelCountdown : public mlg::Entity {
private:
    std::weak_ptr<mlg::Label> countdownLabel;

    float timeToStart = 4.f;
    float startTime = 0.f;

    int lastNumber = 0;

    StartLevelCountdown(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);
public:
    eventpp::CallbackList<void()> OnCountdownStarted;
    eventpp::CallbackList<void()> OnCountdownTick;
    eventpp::CallbackList<void()> OnCountdownFinished;
    
    static std::shared_ptr<StartLevelCountdown> Create(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);

    ~StartLevelCountdown() override;

    void Start() override;
    void Update() override;

    void SetTimeToStart(int timeToStart);

private:
    void InitializeCountdownLabel();
};