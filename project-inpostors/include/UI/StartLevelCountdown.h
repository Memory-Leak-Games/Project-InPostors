#pragma once

#include "Gameplay/Entity.h"
#include "Audio/Assets/AudioAsset.h"
#include <cstddef>

namespace mlg {
    class Label;
}// namespace mlg

class StartLevelCountdown : public mlg::Entity {
private:
    std::weak_ptr<mlg::Label> countdownLabel;

    float timeToStart = 4.f;
    float startTime = 0.f;

    std::shared_ptr<class mlg::AudioAsset> startLevelCountdownSound;
    std::shared_ptr<class mlg::AudioAsset> goLevelCountdownSound;

    bool goSoundPlayed = false;
    bool canPlayStartSound = true;

    StartLevelCountdown(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);
public:
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