#include "Physics/Rigidbody.h"

#include <glm/gtx/matrix_transform_2d.hpp>

#include "Core/Time.h"
#include "Core/Math.h"
#include "Core/RGBA.h"

#include "Physics/Colliders/Collider.h"
#include "Physics/CollisionManager.h"
#include "Physics/Colliders/ColliderShapes.h"

#include "Macros.h"

#include "Rendering/Gizmos/Gizmos.h"
#include "Core/Settings/SettingsManager.h"

namespace mlg {
    Rigidbody::Rigidbody() = default;

    Rigidbody::~Rigidbody() {
        for (auto &collider: colliders) {
            CollisionManager::RemoveCollider(collider);
        }
    };

    // calculate Verlet integration
    void Rigidbody::Integrate() {
        float deltaSecond = Time::GetFixedTimeStep();

        AddForce(-linearDrag * Math::ClampVectorToNormal(linearVelocity) * deltaSecond);
        AddTorque(-angularDrag * Math::Clamp(angularVelocity, -1.f, 1.f) * deltaSecond);

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
        for (auto &collider: colliders) {
            CalculateColliderPosition(collider);
        }
    }

    void Rigidbody::CalculateColliderPosition(std::shared_ptr<Collider> &collider) {
        glm::mat3 rotationMatrix = glm::rotate(glm::mat3{1.f}, rotation);

        glm::vec2 offset = collider->shape->offset;
        glm::vec2 rotatedOffset = glm::vec2(glm::vec3(offset, 1.f) * rotationMatrix);

        collider->shape->position = position + rotatedOffset;
    }


    void Rigidbody::AddForce(glm::vec2 force) {
        newLinearAcceleration += force / mass;

#ifdef DEBUG
        if (SettingsManager::Get<bool>(SettingsType::Debug, "showForces")) {
            glm::vec3 begin{1.f};
            begin.x = position.x;
            begin.z = position.y;

            glm::vec3 end{1.f};
            end.x = position.x + force.x;
            end.z = position.y + force.y;

            Gizmos::DrawLine(begin, end, RGBA::red, true, Time::GetFixedTimeStep());
        }
#endif
    }

    void Rigidbody::AddImpulse(glm::vec2 impulse) {
        AddForce(mass * impulse / Time::GetFixedTimeStep());
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

    void Rigidbody::AddImpulse(glm::vec2 impulse, glm::vec2 localPosition) {
        AddForce(mass * impulse / Time::GetFixedTimeStep(), localPosition);
    }

    const glm::vec2 &Rigidbody::GetPosition() const {
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

    std::weak_ptr<Collider> Rigidbody::AddTrigger(std::unique_ptr<ColliderShape::Shape> shape) {
        auto trigger = std::make_shared<Collider>(this, true, std::move(shape));
        colliders.push_back(trigger);

        CalculateColliderPosition(trigger);
        CollisionManager::AddCollider(trigger);

        return trigger;
    }

    void Rigidbody::ApplyCollisionForce(const CollisionEvent &collision) {
        if (collision.isTrigger)
            return;

        if (collision.collidedRigidbody->isKinematic)
            ApplyCollisionForceWithKinematic(collision);
        else
            ApplyCollisionForceWithDynamic(collision);
    }

    void Rigidbody::ApplyCollisionForceWithDynamic(const CollisionEvent &collision) {
        // prepare physics conditions for readability
        const Rigidbody *anotherRigidbody = collision.collidedRigidbody;

        const glm::vec2 thisMomentum = mass * this->linearVelocity;
        const glm::vec2 anotherMomentum = anotherRigidbody->mass * anotherRigidbody->linearVelocity;

        // before collision
        const float momentumBefore = glm::dot(thisMomentum + anotherMomentum, collision.normal);
        const glm::vec2 relativeVelocities = this->linearVelocity - anotherRigidbody->linearVelocity;

        // after collision (conservation of momentum)
        const float relativeSpeedAfter = -1.f * glm::dot(relativeVelocities, collision.normal);
        const float speedAfter = (momentumBefore + anotherRigidbody->mass * relativeSpeedAfter)
                                 / (mass + anotherRigidbody->mass);

        float impulse = speedAfter - glm::dot(this->linearVelocity, collision.normal);

        // apply bounciness
        impulse *= this->bounciness * anotherRigidbody->bounciness;

        AddImpulse(impulse * collision.normal);

        auto angularForceMultiplier = SettingsManager::Get<float>(SettingsType::Engine,"collisionTorqueMultiplier");
        AddForce(impulse * angularForceMultiplier * collision.normal, collision.position);
    }

    void Rigidbody::ApplyCollisionForceWithKinematic(const CollisionEvent &collision) {
        // prepare physics conditions for readability
        const Rigidbody *anotherRigidbody = collision.collidedRigidbody;

        const glm::vec2 thisMomentum = mass * this->linearVelocity;
        const glm::vec2 anotherMomentum = anotherRigidbody->mass * anotherRigidbody->linearVelocity;

        // before collision
        const float momentumBefore = glm::dot(thisMomentum + anotherMomentum, collision.normal);
        const glm::vec2 relativeVelocities = this->linearVelocity - anotherRigidbody->linearVelocity;

        // after collision (conservation of momentum)
        const float speedAfter = -1.f * glm::dot(relativeVelocities, collision.normal);

        const float bouncinessProduct = this->bounciness * anotherRigidbody->bounciness;

        const float forceToStop = -glm::dot(this->linearVelocity, collision.normal);
        const float forceToBounce = speedAfter;

        const float impulse = forceToStop + forceToBounce * bouncinessProduct;

        //apply impulse (impulse is immediate force (affect velocity not acceleration)
        AddImpulse(impulse * collision.normal);

        auto angularForceMultiplier = SettingsManager::Get<float>(SettingsType::Engine,"collisionTorqueMultiplier");
        AddForce(impulse * angularForceMultiplier * collision.normal, collision.position);
    }

    struct WeakColliderHash {
        size_t operator () (const std::shared_ptr<Collider>& collider) const {
            return (size_t) collider.get();
        }
    };

    void Rigidbody::GetOverlappingColliders(std::vector<std::weak_ptr<Collider>>& output) {
        std::unordered_set<std::shared_ptr<Collider>, WeakColliderHash> outputSet;

        for (const auto& collider: colliders) {
            std::vector<std::shared_ptr<Collider>> nearColliders;
            CollisionManager::FindNear(collider->GetPosition(), collider->GetRadius(), nearColliders);

            // Check collision with near colliders
            for (auto& anotherCollider : nearColliders) {
                // But not with self
                if (collider->GetOwner() == anotherCollider->GetOwner())
                    continue;

                if (collider->DetectCollision(anotherCollider.get())) {
                    outputSet.insert(anotherCollider);
                }
            }
        }

        output.assign(outputSet.begin(), outputSet.end());
    }

} // mlg