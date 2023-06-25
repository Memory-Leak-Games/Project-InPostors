#include "Player/PlayerAudioComponent.h"

#include "Macros.h"
#include "Player/Player.h"

PlayerAudioComponent::PlayerAudioComponent(
        const std::weak_ptr<mlg::Entity>& owner,
        const std::string& name)
    : Component(owner, name) {
    player = std::dynamic_pointer_cast<Player>(owner.lock());
    MLG_ASSERT_MSG(player, "PlayerAudioComponent can only be attached to Player");

    pickUpSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/pick_up.wav");
    dropSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/drop.wav");
    collisionSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/hit.wav");
}

PlayerAudioComponent::~PlayerAudioComponent() = default;

void PlayerAudioComponent::Start() {
    player->OnPickUp.append(
            [this]() {
                pickUpSound->Play();
            });

    player->OnDrop.append(
            [this]() {
                dropSound->Play();
            });
}

void PlayerAudioComponent::Update() {
}
