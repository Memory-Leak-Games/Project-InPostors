#pragma once

namespace mlg {

    namespace ColliderShape {
        class Shape;
    }

    class Rigidbody {
    private:
        glm::vec2 position {0.f};
        float rotation = 0.f;

        glm::vec2 linearVelocity {0.f};
        glm::vec2 currentLinearAcceleration {0.f};
        glm::vec2 newLinearAcceleration {0.f};

        float angularVelocity = 0.f;
        float currentAngularAcceleration = 0.f;
        float newAngularAcceleration = 0.f;

        float mass = 1.f;

        float linearDrag = 0.f;
        float angularDrag = 0.f;

        bool isKinematic = false;

        std::vector<std::shared_ptr<class Collider>> colliders;
    public:
        Rigidbody();
        ~Rigidbody();

        void AddForce(glm::vec2 force);
        void AddForce(glm::vec2 force, glm::vec2 localPosition);

        void AddTorque(float value);

        void AddCollider(std::unique_ptr<ColliderShape::Shape> shape);

        [[nodiscard]] const glm::vec2& GetPosition() const;
        [[nodiscard]] bool GetIsKinematic() const;

        friend class Physics;
        friend class RigidbodyComponent;
        friend class Collider;
    private:
        void Integrate();
        void UpdateColliders();

        void CalculateColliderPosition(std::shared_ptr<Collider>& collider);
    };

} // mlg
