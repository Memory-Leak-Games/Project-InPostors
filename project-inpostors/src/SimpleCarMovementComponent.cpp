#include "SimpleCarMovementComponent.h"

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"

#include "Core/HID/Input.h"
#include "Rendering/Gizmos/Gizmos.h"

#include "Physics/Colliders/ColliderShapes.h"

void SimpleCarMovementComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>();

    rigidbodyComponent.lock()->SetLinearDrag(10.f);
    rigidbodyComponent.lock()->SetAngularDrag(5.f);
}

SimpleCarMovementComponent::SimpleCarMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
        : Component(owner, name) {}

void SimpleCarMovementComponent::PhysicsUpdate() {
    float forward = mlg::Input::GetActionStrength("forward_one");
    forward -= mlg::Input::GetActionStrength("backward_one");

    float right = mlg::Input::GetActionStrength("right_one");
    right -= mlg::Input::GetActionStrength("left_one");

    auto owner = GetOwner().lock();
    auto forwardVector = owner->GetTransform().GetForwardVector();
    glm::vec2 forwardVector2D;
    forwardVector2D.x = forwardVector.x;
    forwardVector2D.y = forwardVector.z;

    rigidbodyComponent.lock()->AddForce(forwardVector2D * forward * 20.f);
    rigidbodyComponent.lock()->AddTorque(right * -6.f);
}

void SimpleCarMovementComponent::Update() {
    auto owner = GetOwner().lock();

    glm::vec3 worldPosition = owner->GetTransform().GetWorldPosition();
    mlg::Gizmos::DrawPoint(worldPosition + owner->GetTransform().GetForwardVector() * 2.f);
}
