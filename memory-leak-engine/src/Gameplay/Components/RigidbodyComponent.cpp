#include "Gameplay/Components/RigidbodyComponent.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsState.h"

#include "Gameplay/Entity.h"
#include "SceneGraph/Transform.h"

namespace mlg {
    RigidbodyComponent::RigidbodyComponent(const std::weak_ptr<Entity>& owner, const std::string& name)
    : Component(owner, name) {
        physicsState = std::make_shared<PhysicsState>();
        Physics::AddState(physicsState);
    }

    RigidbodyComponent::~RigidbodyComponent() {
        Physics::RemoveState(physicsState);
    }

    void RigidbodyComponent::AddForce(glm::vec2 force) {
        physicsState->AddForce(force);
    }

    void RigidbodyComponent::AddForce(glm::vec2 force, glm::vec2 localPosition) {
        physicsState->AddForce(force, localPosition);
    }

    void RigidbodyComponent::AddTorque(float value) {
        physicsState->AddTorque(value);
    }

    void RigidbodyComponent::PhysicsUpdate() {
        Entity* owner = GetOwner().lock().get();
        glm::vec3 ownerPosition = owner->GetTransform().GetPosition();
        ownerPosition.x = physicsState->position.x;
        ownerPosition.z = physicsState->position.y;
        owner->GetTransform().SetPosition(ownerPosition);

        glm::vec3 ownerRotation {};
        ownerRotation.y = physicsState->rotation;
        owner->GetTransform().SetEulerRotation(ownerRotation);
    }

    void RigidbodyComponent::SetLinearDrag(float value) {
        physicsState->linearDrag = value;
    }

    void RigidbodyComponent::SetAngularDrag(float value) {
        physicsState->angularDrag = value;
    }

} // mlg