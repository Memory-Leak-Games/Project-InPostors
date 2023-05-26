#include "Buildings/AutoDestroyComponent.h"

#include "Core/TimerManager.h"

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Entity.h"

#include "Physics/Colliders/Collider.h"
#include "Physics/Rigidbody.h"
#include <memory>
#include <spdlog/spdlog.h>

using Entity = std::shared_ptr<mlg::Entity>;
using Rigidbody = std::shared_ptr<mlg::RigidbodyComponent>;

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

    auto destroyDelegate = []() -> void {
        SPDLOG_DEBUG("Boom");
    };

    rigidbody->OnCollisionEnter.append(
    [this, &destroyDelegate](const mlg::CollisionEvent& event) {
        if (mlg::TimerManager::Get()->IsTimerValid(timerHandle))
            return;

        this->timerHandle = mlg::TimerManager::Get()->SetTimer(
            timeToDestroy, false, destroyDelegate);
    });
}
