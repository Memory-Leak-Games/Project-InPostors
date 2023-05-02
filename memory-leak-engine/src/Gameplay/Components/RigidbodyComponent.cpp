#include "Gameplay/Components/RigidbodyComponent.h"

#include "Core/Settings/SettingsManager.h"

#include "Physics/Physics.h"
#include "Physics/Rigidbody.h"
#include "Physics/Colliders/Collider.h"
#include "Physics/Colliders/ColliderShapes.h"

#include "Gameplay/Entity.h"

#include "Rendering/Gizmos/Gizmos.h"

namespace mlg {
    RigidbodyComponent::RigidbodyComponent(const std::weak_ptr<Entity>& owner, const std::string& name)
    : Component(owner, name) {
        rigidbody = std::make_shared<Rigidbody>();
        Physics::AddRigidbody(rigidbody);

        const glm::vec3 ownerPosition = GetOwner().lock()->GetTransform().GetPosition();
        rigidbody->position.x = ownerPosition.x;
        rigidbody->position.y = ownerPosition.z;

        SetKinematic(GetOwner().lock()->IsStatic());
    }

    RigidbodyComponent::~RigidbodyComponent() {
        Physics::RemoveRigidbody(rigidbody);
    }

    void RigidbodyComponent::AddForce(glm::vec2 force) {
        rigidbody->AddForce(force);
    }

    void RigidbodyComponent::AddForce(glm::vec2 force, glm::vec2 localPosition) {
        rigidbody->AddForce(force, localPosition);
    }

    void RigidbodyComponent::AddTorque(float value) {
        rigidbody->AddTorque(value);
    }

    void RigidbodyComponent::PhysicsUpdate() {
    }

    void RigidbodyComponent::Update() {
        Entity* owner = GetOwner().lock().get();
        glm::vec3 ownerPosition = owner->GetTransform().GetPosition();
        ownerPosition.x = rigidbody->position.x;
        ownerPosition.z = rigidbody->position.y;
        owner->GetTransform().SetPosition(ownerPosition);

        glm::vec3 ownerRotation {};
        ownerRotation.y = rigidbody->rotation;
        owner->GetTransform().SetEulerRotation(ownerRotation);


#ifdef DEBUG
        DebugDraw();
#endif
    }

#ifdef DEBUG
    void RigidbodyComponent::DebugDraw() {
        if (!SettingsManager::Get<bool>(SettingsType::Debug, "showColliders"))
            return;

        for (auto& collider : rigidbody->colliders) {
            glm::vec3 position {0.f};
            position.x = collider->shape->position.x;
            position.z = collider->shape->position.y;

            glm::vec4 gizmoColor = RGBA::green;
            if (rigidbody->GetIsKinematic())
                gizmoColor = RGBA::blue;

            if (collider->isTrigger)
                gizmoColor = RGBA::yellow;

            switch (collider->shape->GetType()) {
                case ColliderShape::ColliderShapeType::Circle: {
                    auto circle = (ColliderShape::Circle*) collider->shape.get();
                    Gizmos::DrawSphere(position, circle->radius, gizmoColor);
                    break;
                }
                case ColliderShape::ColliderShapeType::Rectangle:
                    auto rect = (ColliderShape::Rectangle*) collider->shape.get();
                    Gizmos::DrawBox(position, {rect->size.x, 1.f, rect->size.y}, glm::quat{glm::vec3{0.f}}, gizmoColor);
                    break;
            }
        }
    }
#endif

    void RigidbodyComponent::SetLinearDrag(float value) {
        rigidbody->linearDrag = value;
    }

    void RigidbodyComponent::SetAngularDrag(float value) {
        rigidbody->angularDrag = value;
    }

    void RigidbodyComponent::Start() {
        const glm::vec3 ownerPosition = GetOwner().lock()->GetTransform().GetPosition();
        glm::quat ownerQuat = GetOwner().lock()->GetTransform().GetRotation();
        glm::vec3 ownerRotation = GetOwner().lock()->GetTransform().GetEulerRotation();

        SetKinematic(GetOwner().lock()->IsStatic());
    }

    void RigidbodyComponent::SetKinematic(bool isKinematic) {
        rigidbody->isKinematic = isKinematic;
    }

    void RigidbodyComponent::AddCollider(std::unique_ptr<ColliderShape::Shape> shape) {
        auto collider = rigidbody->AddCollider(std::move(shape)).lock();

#ifdef DEBUG
        if (!SettingsManager::Get<bool>(SettingsType::Debug, "showColliders"))
            return;

        collider->OnCollisionEnter.append([](CollisionEvent event) {
            glm::vec3 position {0.f};
            position.x = event.position.x;
            position.z = event.position.y;
            Gizmos::DrawPoint(position, RGBA::red, true, 0.016);
        });
#endif
    }

    void RigidbodyComponent::AddTrigger(std::unique_ptr<ColliderShape::Shape> shape) {
        auto trigger = rigidbody->AddTrigger(std::move(shape)).lock();

#ifdef DEBUG
        if (!SettingsManager::Get<bool>(SettingsType::Debug, "showColliders"))
            return;

        trigger->OnCollisionEnter.append([](CollisionEvent event) {
            glm::vec3 position {0.f};
            position.x = event.position.x;
            position.z = event.position.y;
            Gizmos::DrawPoint(position, RGBA::yellow, true, 0.016);
        });
#endif
    }

    glm::vec2 RigidbodyComponent::GetLinearVelocity() {
        return rigidbody->linearVelocity;
    }

    glm::vec2 RigidbodyComponent::GetLinearAcceleration() {
        return rigidbody->newLinearAcceleration + rigidbody->currentLinearAcceleration / 2.f;
    }

    float RigidbodyComponent::GetAngularSpeed() {
        return rigidbody->angularVelocity;
    }

    float RigidbodyComponent::GetAngularAcceleration() {
        return rigidbody->newAngularAcceleration + rigidbody->currentAngularAcceleration / 2.f;
    }

    void RigidbodyComponent::SetMass(float mass) {
        rigidbody->mass = mass;
    }

    void RigidbodyComponent::SetBounciness(float bounciness) {
        rigidbody->bounciness = bounciness;
    }

    void RigidbodyComponent::SetAngularVelocity(float newAngularVelocity) {
        rigidbody->angularVelocity = newAngularVelocity;
    }

    void RigidbodyComponent::SetPosition(const glm::vec2 &position) {
        rigidbody->position = position;
    }

    const glm::vec2 &RigidbodyComponent::GetPosition() {
        return rigidbody->position;
    }

    void RigidbodyComponent::SetRotation(float rotation) {
        rigidbody->rotation = rotation;
    }

    float RigidbodyComponent::GetRotation() {
        return rigidbody->rotation;
    }

    glm::vec3 RigidbodyComponent::GetLinearVelocity3D() {
        const glm::vec2 linearVelocity2D = GetLinearVelocity();
        return {linearVelocity2D.x, 0, linearVelocity2D.y};
    }

    glm::vec3 RigidbodyComponent::GetLocalVelocity() {
        return GetOwner().lock()->GetTransform().InverseDirection(GetLinearVelocity3D());
    }


} // mlg