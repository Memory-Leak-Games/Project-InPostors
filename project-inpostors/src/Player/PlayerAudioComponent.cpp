#include "Player/PlayerAudioComponent.h"

#include "Audio/AudioInstance.h"
#include "Car/CarInput.h"
#include "Core/TimerManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"

#include "Physics/Colliders/Collider.h"
#include "Player/Player.h"
#include <spdlog/spdlog.h>

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
    driftSound = std::make_unique<mlg::AudioInstance>("res/audio/sfx/drift.wav");
    driftSound->GetAudioAsset()->SetLooping(true);

    stationarySound = std::make_unique<mlg::AudioInstance>("res/audio/sfx/engine-stationary.wav");
    stationarySound->GetAudioAsset()->SetLooping(true);

    drivingSound = std::make_unique<mlg::AudioInstance>("res/audio/sfx/acceleration.wav");
    drivingSound->GetAudioAsset()->SetLooping(true);
}

PlayerAudioComponent::~PlayerAudioComponent() = default;

void PlayerAudioComponent::Start() {
    playerInput = player->GetComponentByClass<CarInput>();

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
                if (collision.isTrigger)
                    return;

                PlayCollisionSound();
            });
}

void PlayerAudioComponent::Update() {
    HandleDriftSound();
    HandleEngineSound();
}

using Random = effolkronium::random_static;

void PlayerAudioComponent::PlayCollisionSound() {
    constexpr static float collisionSoundDelay = 0.5f;

    if (collisionSoundPlayed) {
        return;
    }

    collisionSound->Play();
    collisionSoundPlayed = true;

    mlg::TimerManager::Get()->SetTimer(
            collisionSoundDelay,
            false,
            [this]() {
                collisionSoundPlayed = false;
            });
}

void PlayerAudioComponent::HandleDriftSound() {
    float angularVelocity = rigidbodyComponent.lock()->GetAngularAcceleration();
    float speed = rigidbodyComponent.lock()->GetSpeed();

    if (speed > 0.1f && std::abs(angularVelocity) > 5.f) {
        if (driftSoundPlaying) {
            return;
        }

        driftSound->Play();
        driftSound->SetVolume(0.25f);
        driftSoundPlaying = true;
    } else {
        driftSound->Stop();
        driftSoundPlaying = false;
    }
}

void PlayerAudioComponent::HandleEngineSound() {
    float forwardInput = playerInput.lock()->GetMovementInput().y;

    if (std::abs(forwardInput) > 0.1f) {
        if (isDriving) {
            return;
        }

        stationarySound->Stop();
        drivingSound->Play();
        drivingSound->SetVolume(0.2f);

        isDriving = true;
    } else {
        if (!isDriving) {
            return;
        }

        stationarySound->Play();
        stationarySound->SetVolume(0.2f);
        drivingSound->Stop();
        isDriving = false;
    }
}
