#pragma once

namespace mlg {

    class PhysicsState {
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

    public:
        PhysicsState();

        void AddForce(glm::vec2 force);
        void AddForce(glm::vec2 force, glm::vec2 localPosition);

        void AddTorque(float value);

        friend class RigidbodyComponent;
        friend class Physics;
    private:
        void Integrate();
    };

} // mlg
