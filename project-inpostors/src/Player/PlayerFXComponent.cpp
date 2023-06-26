#include "Player/PlayerFXComponent.h"

#include "Buildings/Storage.h"
#include "Core/HID/Input.h"
#include "Core/SceneManager/Scene.h"
#include "Core/SceneManager/SceneManager.h"
#include "FX/FXLibrary.h"
#include "Gameplay/Components/ParticleSystemComponent.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Entity.h"
#include "Macros.h"
#include "Managers/TaskManager.h"
#include "Physics/Colliders/Collider.h"
#include "Player/Player.h"
#include "SceneGraph/SceneGraph.h"
#include "Scenes/LevelScene.h"
#include <fstream>
#include <glm/fwd.hpp>
#include <memory>

PlayerFXComponent::PlayerFXComponent(
        const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
    : Component(owner, name) {
    auto sharedOwner = owner.lock();

    rigidbodyComponent =
            sharedOwner->GetComponentByClass<mlg::RigidbodyComponent>().lock();

    auto smoke = FXLibrary::Get("smoke");
    smokeParticleSystem =
            sharedOwner->AddComponent<mlg::ParticleSystemComponent>(
                    "SmokeFX", smoke);
    smokeParticleSystem.lock()->GetTransform().SetPosition(
            glm::vec3(0.0f, 0.5f, 0.0f));
    AddDriftFX();
}

PlayerFXComponent::~PlayerFXComponent() = default;

void PlayerFXComponent::Start() {
    mlg::Scene* scene = mlg::SceneManager::GetCurrentScene();
    LevelScene* levelScene = dynamic_cast<LevelScene*>(scene);

    levelScene->GetTaskManager()->OnTaskFinished.append(
            [this](const TaskData& taskData) {
                OnTaskFinished();
            });

    rigidbodyComponent.lock()->OnCollisionEnter.append(
            [this](mlg::CollisionEvent collisionEvent) {
                if (collisionEvent.isTrigger)
                    return;

                OnCollision(collisionEvent.position);
            });
}

void PlayerFXComponent::Update() {
    collidedThisFrame = false;
    HandleDrift();

#ifdef DEBUG
    if (mlg::Input::IsActionJustPressed("debug_particle")) {
        OnTaskFinished();
    }
#endif
}

void PlayerFXComponent::OnTaskFinished() {
    auto rigidbody = rigidbodyComponent.lock();

    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    const auto& collider = std::find_if(
            overlappingColliders.begin(),
            overlappingColliders.end(),
            [](const auto& collider) {
                std::shared_ptr<mlg::Entity> owner =
                        mlg::RigidbodyComponent::GetColliderOwner(*collider.lock()).lock();
                auto storage = std::dynamic_pointer_cast<Storage>(owner);

                return storage != nullptr;
            });

    if (collider == overlappingColliders.end())
        return;

    auto cash = FXLibrary::Get("cash");
    auto cashParticleSystem =
            GetOwner().lock()->AddComponent<mlg::ParticleSystemComponent>(
                    "CashFX", cash, true);
}

void PlayerFXComponent::OnCollision(glm::vec2 position) {
    if (collidedThisFrame)
        return;

    auto sparkles = FXLibrary::Get("sparkles");
    auto sparklesParticleSystem =
            GetOwner().lock()->AddComponent<mlg::ParticleSystemComponent>(
                    "SparklesFX", sparkles, true);

    collidedThisFrame = true;
}

void PlayerFXComponent::HandleDrift() {
    float angularVelocity = rigidbodyComponent.lock()->GetAngularAcceleration();
    float speed = rigidbodyComponent.lock()->GetSpeed();

    if (speed > 0.1f && std::abs(angularVelocity) > 5.f) {
        driftParticleSystem.lock()->Emit();
    }
}

using json = nlohmann::json;

void PlayerFXComponent::AddDriftFX() {
    std::shared_ptr<Player> player =
            std::dynamic_pointer_cast<Player>(GetOwner().lock());

    std::ifstream configFile(player->GetPlayerData().carData);
    json config = json::parse(configFile);

    glm::vec3 position{0.f, 1.5f, 0.f};

    if (config.contains("particles")) {
        if (config["particles"].contains("drift")) {
            position = {
                    config["particles"]["drift"][0],
                    config["particles"]["drift"][1],
                    config["particles"]["drift"][2],
            };
        }
    }

    driftParticleSystem =
            player->AddComponent<mlg::ParticleSystemComponent>(
                    "DriftFX", FXLibrary::Get("drift"));
    driftParticleSystem.lock()->GetTransform().SetPosition(position);
}
