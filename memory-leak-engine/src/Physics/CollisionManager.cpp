#include "Physics/CollisionManager.h"

#include "Physics/Colliders/Collider.h"
#include "Physics/Rigidbody.h"

#include "Macros.h"

namespace mlg {
    CollisionManager* CollisionManager::instance;

    void CollisionManager::Initialize() {
        if (instance != nullptr)
            return;

        instance = new CollisionManager();

        SPDLOG_INFO("Initializing CollisionManager");
    }

    void CollisionManager::Stop() {
        if (instance == nullptr)
            return;

        delete instance;
        instance = nullptr;

        SPDLOG_INFO("Stopping CollisionManager");
    }

    CollisionManager *CollisionManager::GetInstance() {
        return instance;
    }

// TODO: Hash grid table
    void CollisionManager::DetectCollisions() {
        for (auto& collider : instance->colliders) {
            if (collider.expired())
                continue;

            auto sharedCollider = collider.lock();

            if (sharedCollider->GetOwner()->GetIsKinematic())
                continue;

            // Check collision with every other collider
            for (auto& anotherCollider : instance->colliders) {
                if (anotherCollider.expired())
                    return;

                auto sharedAnotherCollider = anotherCollider.lock();

                // But not with self
                if (sharedCollider->GetOwner() == sharedAnotherCollider->GetOwner())
                    continue;

                if (sharedCollider->DetectCollision(sharedAnotherCollider.get())) {
                    instance->collisionsThisTick.push_back({collider, anotherCollider});
                    sharedCollider->OnCollisionEnter(CollisionEvent{glm::vec2(0.f), sharedAnotherCollider->GetOwner()});
                }
            }
        }
    }

    void CollisionManager::SeparateColliders() {
        for (auto& collision : instance->collisionsThisTick) {
            if (collision.collider.lock()->GetOwner()->GetIsKinematic())
                continue;

            collision.collider.lock()->Separate(collision.anotherCollider.lock().get());
        }

        instance->collisionsThisTick.clear();
    }

    void CollisionManager::AddCollider(std::weak_ptr<Collider> collider) {
        instance->colliders.push_back(collider);
    }

    void CollisionManager::RemoveCollider(std::weak_ptr<Collider> collider) {
        instance->colliders.erase(std::remove_if(instance->colliders.begin(), instance->colliders.end(),
                                         [&collider](const std::weak_ptr<Collider>& entry) {
                                             return collider.lock().get() == entry.lock().get();
                                         }), instance->colliders.end());
    }

} // mlg