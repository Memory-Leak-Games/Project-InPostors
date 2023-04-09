#include "Physics/Rigidbody.h"

#include <glm/gtx/matrix_transform_2d.hpp>

#include "Core/Time.h"
#include "Core/Math.h"

#include "Physics/Colliders/Collider.h"
#include "Physics/CollisionManager.h"
#include "Physics/Colliders/ColliderShapes.h"

#include "Macros.h"

#include "Rendering/Gizmos/Gizmos.h"
#include "Core/Settings/SettingsManager.h"

namespace mlg {
    Rigidbody::Rigidbody() = default;

    Rigidbody::~Rigidbody() {
        for (auto& collider : colliders) {
            CollisionManager::RemoveCollider(collider);
        }
    };

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

    void Rigidbody::UpdateColliders() {
        for (auto& collider : colliders) {
            if (isKinematic)
                continue;

            CalculateColliderPosition(collider);
        }
    }

    void Rigidbody::CalculateColliderPosition(std::shared_ptr<Collider>& collider) {
        glm::mat3 rotationMatrix = glm::rotate(glm::mat3{1.f}, rotation);

        glm::vec2 offset = collider->shape->offset;
        glm::vec2 rotatedOffset = glm::vec2(glm::vec3(offset, 1.f) * rotationMatrix);

        collider->shape->position = position + rotatedOffset;
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

        const float torque = Math::Cross2D(force, localPosition);
        AddTorque(torque);
    }

    const glm::vec2& Rigidbody::GetPosition() const {
        return position;
    }

    bool Rigidbody::GetIsKinematic() const {
        return isKinematic;
    }

    std::weak_ptr<Collider> Rigidbody::AddCollider(std::unique_ptr<ColliderShape::Shape> shape) {
        auto collider = std::make_shared<Collider>(this, std::move(shape));
        colliders.push_back(collider);

        CalculateColliderPosition(collider);
        CollisionManager::AddCollider(collider);

        if (!isKinematic) {
            collider->OnCollisionEnter.append([this](CollisionEvent event) {
                this->ApplyCollisionForce(event);
            });
        }

        return collider;
    }

    void Rigidbody::ApplyCollisionForce(const CollisionEvent& collision) {
        const glm::vec2 forcePosition = collision.position;
        const glm::vec2 forceDirection = Math::SafeNormal(position - collision.collidedRigidbody->position);

        const glm::vec2 thisVelocity = this->linearVelocity;
        const glm::vec2 anotherVelocity = collision.collidedRigidbody->linearVelocity;

        const glm::vec2 relativeVelocity = anotherVelocity - thisVelocity;
        const float relativeSpeed = glm::length(relativeVelocity);

        const float anotherMass = collision.collidedRigidbody->mass;
        const float relativeKineticEnergy = 0.5f * anotherMass * relativeSpeed * relativeSpeed;

        glm::vec2 localForcePosition = collision.position - position;

        auto linearForceMultiplier = SettingsManager::Get<float>(SettingsType::Engine,"collisionForceMultiplier");
        auto angularForceMultiplier = SettingsManager::Get<float>(SettingsType::Engine,"collisionTorqueMultiplier");

        const float linearForceStrength = relativeKineticEnergy * linearForceMultiplier;
        const float torque = Math::Cross2D(forceDirection * relativeKineticEnergy, localForcePosition) * angularForceMultiplier;

        AddForce(forceDirection * linearForceStrength);
        AddTorque(torque);
    }

} // mlg