#include "CarMovementComponent.h"

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/HID/Input.h"
#include "Rendering/Gizmos/Gizmos.h"

#include "Physics/Colliders/ColliderShapes.h"

#include "Core/Math.h"

void CarMovementComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>();

    rigidbodyComponent.lock()->SetLinearDrag(10.f);
    rigidbodyComponent.lock()->SetAngularDrag(5.f);

    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>();
}

CarMovementComponent::CarMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
        : Component(owner, name) {}

void CarMovementComponent::PhysicsUpdate() {
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

void CarMovementComponent::Update() {
    auto owner = GetOwner().lock();

    float angularSpeed = glm::degrees(rigidbodyComponent.lock()->GetAngularSpeed());

    float bodySkew = mlg::Math::Clamp(-angularSpeed / 10.f, -15.f, 15.f);

    staticMeshComponent.lock()->GetTransform().SetRotation({{0.f, 0.f, glm::radians(bodySkew)}});
}
