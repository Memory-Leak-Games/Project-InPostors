#include "Player/PlayerAudioComponent.h"

#include "Core/TimerManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"

#include "Physics/Colliders/Collider.h"
#include "Player/Player.h"
#include <cstddef>

PlayerAudioComponent::PlayerAudioComponent(
        const std::weak_ptr<mlg::Entity>& owner,
        const std::string& name)
    : Component(owner, name) {
    player = std::dynamic_pointer_cast<Player>(owner.lock());
    MLG_ASSERT_MSG(player, "PlayerAudioComponent can only be attached to Player");

    rigidbodyComponent = player->GetComponentByClass<mlg::RigidbodyComponent>();

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

    rigidbodyComponent.lock()->OnCollisionEnter.append(
            [this](const mlg::CollisionEvent& collision) {
                PlayCollisionSound();
            });
}

void PlayerAudioComponent::Update() {
}

using Random = effolkronium::random_static;

void PlayerAudioComponent::PlayCollisionSound() {
    constexpr static float collisionSoundDelay = 0.1f;
    constexpr static float collisionSoundMaxPitch = 1.5f;
    constexpr static float collisionSoundMinPitch = 0.5f;

    if (collisionSoundPlayed) {
        return;
    }

    float pitch = Random::get(collisionSoundMinPitch, collisionSoundMaxPitch);

    collisionSound->SetSingleInstance();
    collisionSound->Play();
    collisionSoundPlayed = true;


    mlg::TimerManager::Get()->SetTimer(
            collisionSoundDelay,
            false,
            [this]() {
                collisionSoundPlayed = false;
            });
}
