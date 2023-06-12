#pragma once

#include "Gameplay/Entity.h"
#include <memory>

namespace mlg {
    class AudioAsset;
}

class AudioManager : public mlg::Entity {
    std::shared_ptr<mlg::AudioAsset> finishTaskSound;

    std::shared_ptr<class mlg::AudioAsset> startLevelCountdownSound;
    std::shared_ptr<class mlg::AudioAsset> finishCountdownSound;

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
    void Update() override;
};