#include "Physics/Rigidbody.h"

#include "Core/Time.h"
#include "Core/Math.h"

#include "include/Physics/Colliders/Collider.h"

#include "Macros.h"

namespace mlg {
    Rigidbody::Rigidbody() = default;
    Rigidbody::~Rigidbody() = default;

    // calculate Verlet integration
    void Rigidbody::Integrate() {
        float deltaSecond = Time::GetFixedTimeStep();

        AddForce(-linearDrag * Math::ClampVectorToNormal(linearVelocity));
        AddTorque(-angularDrag * Math::Clamp(angularVelocity, -1.f, 1.f));

        // Linear
        position += linearVelocity * deltaSecond + (currentLinearAcceleration * deltaSecond * deltaSecond) * 0.5f;
        linearVelocity += (currentLinearAcceleration + newLinearAcceleration) * deltaSecond * 0.5f;
        currentLinearAcceleration = newLinearAcceleration;
        newLinearAcceleration = glm::vec2(0.f);

        // Angular
        rotation += angularVelocity * deltaSecond + (currentAngularAcceleration * deltaSecond * deltaSecond) * 0.5f;
        angularVelocity += (currentAngularAcceleration + newAngularAcceleration) * deltaSecond * 0.5f;
        currentAngularAcceleration = newAngularAcceleration;
        newAngularAcceleration = 0.f;
    }

    void Rigidbody::AddForce(glm::vec2 force) {
        newLinearAcceleration += force / mass;
    }

    void Rigidbody::AddTorque(float value) {
        const float momentOfInertia = 0.5f * mass;

        newAngularAcceleration += value / momentOfInertia;
    }

    void Rigidbody::AddForce(glm::vec2 force, glm::vec2 localPosition) {
        AddForce(force);

        const float radius = glm::length(localPosition);
        const float torque = Math::Cross2D(force, localPosition);
        AddTorque(torque);
    }

    const glm::vec2 &Rigidbody::GetPosition() {
        return position;
    }
} // mlg