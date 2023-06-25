#pragma once

#include "Gameplay/Component.h"
#include <memory>

namespace mlg {
    class RigidbodyComponent;
    class AudioAsset;
}// namespace mlg

class PlayerAudioComponent : public mlg::Component {
private:
    std::shared_ptr<class Player> player;

    std::weak_ptr<mlg::RigidbodyComponent> rigidbodyComponent;

    std::shared_ptr<mlg::AudioAsset> pickUpSound;
    std::shared_ptr<mlg::AudioAsset> dropSound;

    std::shared_ptr<mlg::AudioAsset> collisionSound;

    bool collisionSoundPlayed = false;

public:
    PlayerAudioComponent(
            const std::weak_ptr<mlg::Entity>& owner,
            const std::string& name);
    ~PlayerAudioComponent() override;

    void Start() override;
    void Update() override;
};
