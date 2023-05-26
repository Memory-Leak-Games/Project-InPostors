#include "Buildings/AutoDestroyComponent.h"

#include "Core/TimerManager.h"

#include "FX/FXLibrary.h"
#include "Gameplay/Components/ParticleSystemComponent.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Entity.h"

#include "Physics/Colliders/Collider.h"
#include "Physics/Rigidbody.h"
#include <memory>
#include <spdlog/spdlog.h>

using Entity = std::shared_ptr<mlg::Entity>;
using Rigidbody = std::shared_ptr<mlg::RigidbodyComponent>;
using Particles = std::shared_ptr<mlg::ParticleSystemComponent>;
using StaticMesh = std::shared_ptr<mlg::StaticMeshComponent>;

AutoDestroyComponent::AutoDestroyComponent(
        std::weak_ptr<mlg::Entity> owner,
        std::string name,
        float timeToDestory)
    : Component(std::move(owner), std::move(name)),
      timeToDestroy(timeToDestory),
      timerHandle(0) {}

void AutoDestroyComponent::Start() {
    Entity owner = GetOwner().lock();
    Rigidbody rigidbody = owner->GetComponentByClass<mlg::RigidbodyComponent>().lock();
    StaticMesh mesh = owner->GetComponentByClass<mlg::StaticMeshComponent>().lock();

    auto destroyDelegate = [owner]() -> void {
        owner->QueueForDeletion();
    };

    rigidbody->OnCollisionEnter.append(
    [this, owner, mesh, destroyDelegate](const mlg::CollisionEvent& event) {
        if (mlg::TimerManager::Get()->IsTimerValid(timerHandle))
            return;

        owner->AddComponent<mlg::ParticleSystemComponent>("DestroyFX", FXLibrary::Get("destroy")).lock();
        mesh->SetVisible(false);

        this->timerHandle = mlg::TimerManager::Get()->SetTimer(
            timeToDestroy, false, destroyDelegate);
    });
}
