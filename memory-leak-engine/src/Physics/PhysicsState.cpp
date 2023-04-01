#include "Physics/PhysicsState.h"

#include "Core/Time.h"
#include "Core/Math.h"

namespace mlg {
    PhysicsState::PhysicsState()
            : position(0.f), velocity(0.f), angularVelocity(0.f), mass(1.f) {

    }

    const glm::vec2& PhysicsState::GetPosition() const {
        return position;
    }

    const glm::vec2& PhysicsState::GetVelocity() const {
        return velocity;
    }

    float PhysicsState::GetAngularVelocity() const {
        return angularVelocity;
    }

    float PhysicsState::GetMass() const {
        return mass;
    }

    // calculate Verlet integration
    void PhysicsState::Integrate() {
        float deltaSecond = Time::GetFixedDeltaSeconds();

        // Linear
        position += velocity * deltaSecond + (currentAcceleration * deltaSecond * deltaSecond) * 0.5f;
        velocity += (currentAcceleration + newAcceleration) * deltaSecond * 0.5f;
        currentAcceleration = newAcceleration;

        // Angular
        rotation += angularVelocity * deltaSecond + (currentAngularAcceleration * deltaSecond * deltaSecond) * 0.5f;
        angularVelocity += (currentAngularAcceleration + newAngularAcceleration) * deltaSecond * 0.5f;
        currentAngularAcceleration = newAngularAcceleration;
    }

    void PhysicsState::AddForce(glm::vec2 force) {
        newAcceleration += force / mass;
    }

    void PhysicsState::AddTorque(float value) {
        newAcceleration += value / mass;
    }

    void PhysicsState::AddForce(glm::vec2 force, glm::vec2 localPosition) {
        AddForce(force);

        const float radius = glm::length(localPosition);

        const float torque = Math::Cross2D(force, localPosition);
        const float momentOfInertia = mass * radius * radius;

        newAngularAcceleration += torque / momentOfInertia;
    }
} // mlg