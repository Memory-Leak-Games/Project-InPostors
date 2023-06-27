#pragma once

#include "Gameplay/Entity.h"
#include <memory>

namespace mlg {
    class AudioAsset;
}

class AudioManager : public mlg::Entity {
    std::shared_ptr<mlg::AudioAsset> finishTaskSound;
    std::shared_ptr<mlg::AudioAsset> mediumFinishTaskSound;
    std::shared_ptr<mlg::AudioAsset> superFinishTaskSound;

    std::shared_ptr<class mlg::AudioAsset> startLevelCountdownSound;
    std::shared_ptr<class mlg::AudioAsset> finishCountdownSound;

    std::shared_ptr<mlg::AudioAsset> clockCountdownSound;
    std::shared_ptr<mlg::AudioAsset> boxingBellSound;

    std::shared_ptr<mlg::AudioAsset> cityAmbientSound;
    std::shared_ptr<mlg::AudioAsset> levelMusic;

    // used to countdown timer sounds
    int lastWholeSecondLeft = -1;

    int currentSoundIndex = 0;
    float minDelayBetweenRandomSounds = 5.f;
    float maxDelayBetweenRandomSounds = 10.f;
    std::vector<std::shared_ptr<mlg::AudioAsset>> randomSounds;

    size_t randomSoundTimer = 0;


    AudioManager(uint64_t id,
                 const std::string& name,
                 bool isStatic,
                 mlg::Transform* parent);

public:
    ~AudioManager() override;

    static std::shared_ptr<AudioManager> Create(
            uint64_t id,
            const std::string& name,
            bool isStatic,
            mlg::Transform* parent);

    void Start() override;
    void Stop() override;

    void SetTimeLeft(float timeLeft);

private:
    void LoadRandomSounds();
    void StartRandomSoundTimer();
};