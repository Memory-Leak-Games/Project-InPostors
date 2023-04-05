
#pragma once

namespace mlg {

    enum class ColliderType {
        Circle,
        Square
    };

    struct CollisionEvent {
        glm::vec2 position;
        class Rigidbody* collidedRigidbody;
    };

    class Collider {
    public:
        eventpp::CallbackList<void(CollisionEvent)> OnCollisionEnter;

    protected:
        Rigidbody* owner;

    public:
        Collider() = default;

        virtual ColliderType GetColliderType() = 0;
        virtual float GetRadius() = 0;

        friend class CollisionDetection;
    };

}// namespace mlg
