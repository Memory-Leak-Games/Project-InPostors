#pragma once

#include "ColliderShapes.h"
#include "Physics/Rigidbody.h"

namespace mlg {

    namespace ColliderShape {
        class Shape;
    }

    struct CollisionEvent {
        glm::vec2 position;
        glm::vec2 separationVector;
        glm::vec2 normal;
        const class Rigidbody* collidedRigidbody;
    };

    class Collider {
    public:
        eventpp::CallbackList<void(CollisionEvent)> OnCollisionEnter;

    private:
        Rigidbody* owner;
        std::unique_ptr<ColliderShape::Shape> shape;

        // HashGridClient
        glm::vec2 minCoord;
        glm::vec2 maxCoord;

        uint32_t queryID = -1;

    public:
        Collider(Rigidbody* owner, std::unique_ptr<ColliderShape::Shape> shape);
        ~Collider();

        bool DetectCollision(Collider* anotherCollider);
        void Separate(Collider* anotherCollider, glm::vec2 SeparationVector);

        glm::vec2 CalculateSeparation(Collider* anotherCollider);
        glm::vec2 FindCollisionPoint(const glm::vec2& anotherPosition);

        const Rigidbody* GetOwner() const;
        glm::vec2 GetPosition() const;
        float GetRadius() const;

        friend class RigidbodyComponent;
        friend class Rigidbody;

        friend class SpacialHashGrid;

    private:
        bool DetectCollisionAsCircle(Collider* anotherCollider);
        bool DetectCollisionAsRectangle(Collider* anotherCollider);

        glm::vec2 CalculateSeparationAsCircle(Collider* anotherCollider);
        glm::vec2 CalculateSeparationAsRectangle(Collider* anotherCollider);
    };

}// namespace mlg
