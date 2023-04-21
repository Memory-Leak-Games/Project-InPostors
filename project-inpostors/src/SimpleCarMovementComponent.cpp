#include "CarMovementComponent.h"

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/HID/Input.h"
#include "Rendering/Gizmos/Gizmos.h"
#include "Core/Time.h"

#include "Physics/Colliders/ColliderShapes.h"

#include "Core/Math.h"

void CarMovementComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>();

    rigidbodyComponent.lock()->SetLinearDrag(10.f);
    rigidbodyComponent.lock()->SetAngularDrag(5.f);

    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>();
}

CarMovementComponent::CarMovementComponent(const std::weak_ptr<mlg::Entity> &owner, const std::string &name)
        : Component(owner, name) {}

void CarMovementComponent::PhysicsUpdate() {
    forward = mlg::Input::GetActionStrength("forward_one");
    forward -= mlg::Input::GetActionStrength("backward_one");

    right = mlg::Input::GetActionStrength("right_one");
    right -= mlg::Input::GetActionStrength("left_one");

    HandleEngineAndBraking();
    HandleSteering();
    HandleSideDrag();
    CounterTorque();

}

void CarMovementComponent::Update() {
    auto owner = GetOwner().lock();

    float angularSpeed = glm::degrees(rigidbodyComponent.lock()->GetAngularSpeed());

    float bodySkew = mlg::Math::Clamp(-angularSpeed / 10.f, -15.f, 15.f);

    staticMeshComponent.lock()->GetTransform().SetRotation({{0.f, 0.f, glm::radians(bodySkew)}});
}

void CarMovementComponent::HandleEngineAndBraking() {
    auto owner = GetOwner().lock();
    auto forwardVector = owner->GetTransform().GetForwardVector();
    glm::vec2 forwardVector2D;
    forwardVector2D.x = forwardVector.x;
    forwardVector2D.y = forwardVector.z;

    auto linearVelocity2D = rigidbodyComponent.lock()->GetLinearVelocity();
    glm::vec3 linearVelocityTransformed(linearVelocity2D.x, 0.f, linearVelocity2D.y);
    auto localVelocity = glm::inverse((glm::mat3(owner->GetTransform().GetLocalMatrix()))) * linearVelocityTransformed;
    float speed = glm::length(localVelocity);

    float targetAccelerationForce = 0.f;

    // accelerating forward
    if (speed < maxSpeed && forward > 0.1) {
        targetAccelerationForce += acceleration * forward * mlg::Time::GetFixedTimeStep();
    }

    if (speed > -backwardMaxSpeed && forward < -0.1) {
        targetAccelerationForce += acceleration * forward * mlg::Time::GetFixedTimeStep();
    }

    // engine handling
    if (glm::abs(forward) < 0.1f) {
        targetAccelerationForce =
                -1.f * std::clamp(localVelocity.z, 0.f, 1.f) * engineHandling * mlg::Time::GetFixedTimeStep();
    }

    // handling when velocity direction != movement direction
    if ((std::signbit(localVelocity.z) != std::signbit(forward)) &&
        (glm::abs(localVelocity.z) > 0.1) && (glm::abs(forward) > 0.1)) {
        targetAccelerationForce = -1.f * std::clamp(localVelocity.z, 0.f, 1.f) * handling * mlg::Time::GetFixedTimeStep();
    }

    // TODO: this is quick hack not solution :3
    currentAccelerationForce = mlg::Math::Lerp(currentAccelerationForce, targetAccelerationForce, 10.f * mlg::Time::GetFixedTimeStep());
    rigidbodyComponent.lock()->AddForce(currentAccelerationForce * forwardVector2D);
}

void CarMovementComponent::HandleSteering() {
    auto owner = GetOwner().lock();
    auto linearVelocity2D = rigidbodyComponent.lock()->GetLinearVelocity();
    glm::vec3 linearVelocityTransformed(linearVelocity2D.x, 0, linearVelocity2D.y);
    auto localVelocity = glm::inverse((glm::mat3(owner->GetTransform().GetLocalMatrix()))) * linearVelocityTransformed;

    if (glm::abs(localVelocity.z) < 0.1) {
        return;
    }

    float steeringInput = -right;
    auto steeringSpeedFactor = std::clamp(localVelocity.z / rotationRadius, -1.f, 1.f);
    auto steeringTorque = steeringInput * rotationSpeed * steeringSpeedFactor * mlg::Time::GetFixedTimeStep();

    auto angularVelocity = rigidbodyComponent.lock()->GetAngularSpeed();

    if (glm::abs(angularVelocity) < glm::abs(steeringTorque)) {
        angularVelocity = steeringTorque;
        rigidbodyComponent.lock()->SetAngularVelocity(angularVelocity);
    }
}

void CarMovementComponent::HandleSideDrag() {
    auto owner = GetOwner().lock();
    auto rightVector = owner->GetTransform().GetRightVector();

    auto linearVelocity2D = rigidbodyComponent.lock()->GetLinearVelocity();
    glm::vec3 linearVelocityTransformed(linearVelocity2D.x, 0, linearVelocity2D.y);
    auto localVelocity = glm::inverse((glm::mat3(owner->GetTransform().GetLocalMatrix()))) * linearVelocityTransformed;

    auto sideDragStrength = localVelocity.x * sideDrag * mlg::Time::GetFixedTimeStep();
    rigidbodyComponent.lock()->AddForce(rightVector * sideDragStrength);
}

void CarMovementComponent::CounterTorque() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
    const glm::vec3 upVector = owner->GetTransform().GetUpVector();

    const float rotationVelocity = rigidbodyComponent.lock()->GetAngularSpeed();

    if (glm::abs(rotationVelocity) < 0.1)
        return;

    const float torqueStrength = std::clamp(-rotationVelocity, -1.f, 1.f) * counterTorque;
    rigidbodyComponent.lock()->AddTorque(upVector.y * torqueStrength);
}
