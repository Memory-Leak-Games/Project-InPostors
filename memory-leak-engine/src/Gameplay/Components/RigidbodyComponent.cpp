#include "Gameplay/Components/RigidbodyComponent.h"

#include "Physics/Physics.h"
#include "Physics/Rigidbody.h"

#include "Gameplay/Entity.h"
#include "SceneGraph/Transform.h"

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
    }

    void RigidbodyComponent::SetLinearDrag(float value) {
        rigidbody->linearDrag = value;
    }

    void RigidbodyComponent::SetAngularDrag(float value) {
        rigidbody->angularDrag = value;
    }


} // mlg