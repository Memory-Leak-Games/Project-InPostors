#pragma once

namespace mlg {

    class PhysicsState {
    private:
        glm::vec2 position;
        float rotation;

        glm::vec2 velocity;
        glm::vec2 currentAcceleration;
        glm::vec2 newAcceleration;

        float angularVelocity;
        float currentAngularAcceleration;
        float newAngularAcceleration;

        float mass;
    public:
        PhysicsState();

        void AddForce(glm::vec2 force);
        void AddForce(glm::vec2 force, glm::vec2 localPosition);

        void AddTorque(float value);

        [[nodiscard]] const glm::vec2& GetPosition() const;
        [[nodiscard]] const glm::vec2& GetVelocity() const;
        [[nodiscard]] float GetAngularVelocity() const;
        [[nodiscard]] float GetMass() const;

        friend class Physics;
    private:
        void Integrate();
    };

} // mlg
