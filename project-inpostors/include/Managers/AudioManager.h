#pragma once

#include "Gameplay/Entity.h"
#include <memory>

namespace mlg {
    class AudioAsset;
}

class AudioManager : public mlg::Entity {
    std::shared_ptr<mlg::AudioAsset> finishTaskSound;

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