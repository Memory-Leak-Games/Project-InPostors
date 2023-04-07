#pragma once

namespace mlg {

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

//        std::vector<std::unique_ptr<class Collider>> collider;

    public:
        Rigidbody();

        void AddForce(glm::vec2 force);
        void AddForce(glm::vec2 force, glm::vec2 localPosition);

        void AddTorque(float value);

        const glm::vec2& GetPosition();

        friend class Physics;
        friend class CollisionResponses;
        friend class RigidbodyComponent;
    private:
        void Integrate();
    };

} // mlg