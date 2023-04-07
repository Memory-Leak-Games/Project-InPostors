#include "Gameplay/Components/RigidbodyComponent.h"

#include "Physics/Physics.h"
#include "Physics/Rigidbody.h"
#include "Physics/Colliders/Collider.h"
#include "Physics/Colliders/ColliderShapes.h"

#include "Gameplay/Entity.h"
#include "SceneGraph/Transform.h"

#include "Rendering/Gizmos/Gizmos.h"

namespace mlg {
    RigidbodyComponent::RigidbodyComponent(const std::weak_ptr<Entity>& owner, const std::string& name)
    : Component(owner, name) {
        rigidbody = std::make_shared<Rigidbody>();
        Physics::AddRigidbody(rigidbody);
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

        // TODO: DEBUG SETTINGS AND BINDING TO SWITCH
#ifdef DEBUG
        for (auto& collider : rigidbody->colliders) {
            glm::vec3 position {0.f};
            position.x = collider->shape->position.x;
            position.z = collider->shape->position.y;

            switch (collider->shape->GetType()) {
                case ColliderShape::ColliderShapeType::Circle: {
                    auto circle = (ColliderShape::Circle*) collider->shape.get();
                    Gizmos::DrawSphere(position, circle->radius);
                    break;
                }
                case ColliderShape::ColliderShapeType::Rectangle:
                    auto rect = (ColliderShape::Rectangle*) collider->shape.get();
                    Gizmos::DrawBox(position, {rect->size.x, 1.f, rect->size.y});
                    break;
            }
        }
#endif
    }

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


} // mlg