#include "SimpleCarMovementComponent.h"

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"

#include "Core/HID/Input.h"
#include "Rendering/Gizmos/Gizmos.h"

void SimpleCarMovementComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>();

    rigidbodyComponent.lock()->SetLinearDrag(10.f);
    rigidbodyComponent.lock()->SetAngularDrag(5.f);
}

void SimpleCarMovementComponent::Update() {
    float forward = mlg::Input::GetActionStrength("forward_one");
    forward -= mlg::Input::GetActionStrength("backward_one");

    float right = mlg::Input::GetActionStrength("right_one");
    right -= mlg::Input::GetActionStrength("left_one");

    auto owner = GetOwner().lock();
    auto forwardVector = owner->GetTransform().GetForwardVector();
    glm::vec2 forwardVector2D;
    forwardVector2D.x = forwardVector.x;
    forwardVector2D.y = forwardVector.z;

    mlg::Gizmos::DrawPoint(owner->GetTransform().GetWorldPosition() + owner->GetTransform().GetForwardVector() * 2.f);

    rigidbodyComponent.lock()->AddForce(forwardVector2D * forward * 10.f);
    rigidbodyComponent.lock()->AddTorque(right * -3.f);
}

SimpleCarMovementComponent::SimpleCarMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
        : Component(owner, name) {}
