#include "Gameplay/Components/RigidbodyComponent.h"

#include "Core/Settings/SettingsManager.h"

#include "Physics/Physics.h"
#include "Physics/Rigidbody.h"
#include "Physics/Colliders/Collider.h"
#include "Physics/Colliders/ColliderShapes.h"

#include "Gameplay/Entity.h"

#include "Rendering/Gizmos/Gizmos.h"
#include "include/Rendering/Gizmos/Colors.h"

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

            glm::vec4 gizmoColor {0.f, 1.f, 0.f, 1.f};
            if (rigidbody->GetIsKinematic())
                gizmoColor = {0.f, 0.f, 1.f, 1.f};

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
        rigidbody->position.x = ownerPosition.x;
        rigidbody->position.y = ownerPosition.z;

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
            Gizmos::DrawPoint(position, Colors::Red, true, 0.1);
        });
#endif
    }


} // mlg