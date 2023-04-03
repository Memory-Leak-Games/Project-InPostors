
#pragma once

namespace mlg {

    enum class ColliderType {
        Circle,
        Square
    };

    struct CollisionResponses {
        glm::vec2 position;
        class Rigidbody* collidedRigidbody;
    };

    class Collider {
    public:
        eventpp::CallbackList<void(CollisionResponses)> OnCollisionEnter;

    private:
        Rigidbody* owner;

    public:
        Collider() = default;

        virtual ColliderType GetColliderType() = 0;
        virtual float GetRadius() = 0;
    };

}// namespace mlg
