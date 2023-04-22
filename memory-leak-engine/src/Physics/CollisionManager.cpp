#include "Physics/CollisionManager.h"

#include "Physics/Colliders/Collider.h"
#include "Physics/Rigidbody.h"

#include "Core/Math.h"
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

    CollisionManager* CollisionManager::GetInstance() {
        return instance;
    }

    void CollisionManager::UpdateSpacialGrid() {
        ZoneScopedC(tracy::Color::ColorType::Green);

        for (auto& collider : instance->colliders) {
            instance->spacialHashGrid.Update(collider.lock());
        }
    }

    void CollisionManager::DetectCollisions() {
        ZoneScopedC(tracy::Color::ColorType::Green);

        for (auto& collider : instance->colliders) {
            if (collider.expired())
                continue;

            auto sharedCollider = collider.lock();

            if (sharedCollider->GetOwner()->GetIsKinematic())
                continue;

            std::vector<std::shared_ptr<Collider>> nearColliders;
            instance->spacialHashGrid.FindNear(collider.lock()->GetPosition(),
                                               collider.lock()->GetRadius(), nearColliders);

            // Check collision with near colliders
            for (auto& anotherCollider : nearColliders) {
                // But not with self
                if (sharedCollider->GetOwner() == anotherCollider->GetOwner())
                    continue;

                if (sharedCollider->DetectCollision(anotherCollider.get())) {
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

    void CollisionManager::AddCollider(const std::weak_ptr<Collider>& collider) {
        instance->colliders.push_back(collider);
        instance->spacialHashGrid.AddClient(collider.lock());
    }

    void CollisionManager::RemoveCollider(std::weak_ptr<Collider> collider) {
        instance->colliders.erase(std::remove_if(instance->colliders.begin(), instance->colliders.end(),
                                                 [&collider](const std::weak_ptr<Collider>& entry) {
                                                     return collider.lock().get() == entry.lock().get();
                                                 }),
                                  instance->colliders.end());

        instance->spacialHashGrid.RemoveClient(collider.lock());
    }

    // todo: Move out spacial grid initialization
    CollisionManager::CollisionManager()
        : spacialHashGrid(glm::vec2{-50.f, -50.f}, glm::vec2{50.f, 50.f}, glm::ivec2{100, 100}) {
    }


}// namespace mlg