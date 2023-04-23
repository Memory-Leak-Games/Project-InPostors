#include "Physics/CollisionManager.h"

#include "Physics/Colliders/Collider.h"
#include "Physics/Rigidbody.h"

#include "Macros.h"
#include "Core/Math.h"

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
        ZoneScopedC(tracy::Color::ColorType::Green);

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
                }
            }
        }
    }

    void CollisionManager::SolveCollisions() {
        ZoneScopedC(tracy::Color::ColorType::Green);

        for (auto& collision : instance->collisionsThisTick) {
            if (collision.collider.lock()->GetOwner()->GetIsKinematic())
                continue;

            auto collider = collision.collider.lock();
            auto anotherCollider = collision.anotherCollider.lock();

            glm::vec2 separationVector = collider->CalculateSeparation(anotherCollider.get());
            collider->Separate(anotherCollider.get(), separationVector);

            glm::vec2 collisionPoint = collider->FindCollisionPoint(anotherCollider->GetOwner()->GetPosition());
            glm::vec2 normal = Math::SafeNormal(collider->GetPosition() - collisionPoint);
            collider->OnCollisionEnter({collisionPoint, separationVector, normal, anotherCollider->GetOwner()});
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