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
#include "SceneGraph/SceneGraph.h"
#include "Scenes/LevelScene.h"
#include <glm/fwd.hpp>

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