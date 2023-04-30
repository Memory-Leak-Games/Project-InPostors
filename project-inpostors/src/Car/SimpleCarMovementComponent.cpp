#include "include/Car/CarMovementComponent.h"

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/HID/Input.h"
#include "Rendering/Gizmos/Gizmos.h"
#include "Core/Time.h"

#include "Physics/Colliders/ColliderShapes.h"

#include "Car/CarInput.h"

#include "Core/Math.h"

void CarMovementComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>().lock();

    rigidbodyComponent->SetLinearDrag(10.f);
    rigidbodyComponent->SetAngularDrag(5.f);

    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>().lock();

    carInput = GetOwner().lock()->GetComponentByClass<CarInput>().lock();

    currentAccelerationForce = 0.f;
}

CarMovementComponent::CarMovementComponent(const std::weak_ptr<mlg::Entity> &owner, const std::string &name)
    : Component(owner, name) {}

void CarMovementComponent::PhysicsUpdate() {
    HandleEngineAndBraking();
    HandleSteering();
    HandleSideDrag();
    CounterTorque();

}

void CarMovementComponent::Update() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();

    float angularSpeed = glm::degrees(rigidbodyComponent->GetAngularSpeed());
    float bodySkew = mlg::Math::Clamp(-angularSpeed / 10.f, -15.f, 15.f);

    staticMeshComponent->GetTransform().SetRotation({{0.f, 0.f, glm::radians(bodySkew)}});
}

void CarMovementComponent::HandleEngineAndBraking() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();

    const glm::vec3 forwardVector = owner->GetTransform().GetForwardVector();
    glm::vec2 forwardVector2D;
    forwardVector2D.x = forwardVector.x;
    forwardVector2D.y = forwardVector.z;

    const glm::vec2 linearVelocity2D = rigidbodyComponent->GetLinearVelocity();
    const glm::vec3 linearVelocityTransformed(linearVelocity2D.x, 0.f, linearVelocity2D.y);
    const glm::vec3 localVelocity = glm::inverse((glm::mat3(owner->GetTransform().GetLocalMatrix()))) * linearVelocityTransformed;

    const glm::vec2 movementInput = carInput->GetMovementInput();

    float targetAccelerationForce = acceleration * movementInput.y * mlg::Time::GetFixedTimeStep();
    targetAccelerationForce = mlg::Math::Lerp(targetAccelerationForce, std::copysign(maxSpeed, movementInput.y), movementInput.y > 0 ? movementInput.y : -movementInput.y);
    targetAccelerationForce = mlg::Math::Lerp(targetAccelerationForce, std::copysign(backwardMaxSpeed, movementInput.y), movementInput.y < 0 ? movementInput.y : -movementInput.y);

    // engine handling
    if (glm::abs(movementInput.y) < 0.1f) {
        targetAccelerationForce =
                -1.f * std::clamp(localVelocity.z, 0.f, 1.f) * engineHandling * mlg::Time::GetFixedTimeStep();
    }

    // handling when velocity direction != movement direction
    if ((std::signbit(localVelocity.z) != std::signbit(movementInput.y)) &&
        (glm::abs(localVelocity.z) > 0.1) && (glm::abs(movementInput.y) > 0.1)) {
        targetAccelerationForce = -1.f * std::clamp(localVelocity.z, 0.f, 1.f) * handling * mlg::Time::GetFixedTimeStep();
    }

    // TODO: this is quick hack not solution :3
    currentAccelerationForce = mlg::Math::Lerp(currentAccelerationForce, targetAccelerationForce, 10.f * mlg::Time::GetFixedTimeStep());
    currentAccelerationForce = glm::clamp(currentAccelerationForce, -100.f, 100.f);
    rigidbodyComponent->AddForce(currentAccelerationForce * forwardVector2D);
}

void CarMovementComponent::HandleSteering() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();

    glm::vec2 linearVelocity2D = rigidbodyComponent->GetLinearVelocity();
    glm::vec3 linearVelocityTransformed(linearVelocity2D.x, 0, linearVelocity2D.y);
    glm::vec3 localVelocity = glm::inverse((glm::mat3(owner->GetTransform().GetLocalMatrix()))) * linearVelocityTransformed;

    if (glm::abs(localVelocity.z) < 0.1) {
        return;
    }

    const glm::vec2 movementInput = carInput->GetMovementInput();

    float steeringInput = -movementInput.x;
    float steeringSpeedFactor = std::clamp(localVelocity.z / rotationRadius, -1.f, 1.f);
    float steeringTorque = steeringInput * rotationSpeed * steeringSpeedFactor * mlg::Time::GetFixedTimeStep();

    float angularVelocity = rigidbodyComponent->GetAngularSpeed();

    if (glm::abs(angularVelocity) < glm::abs(steeringTorque)) {
        angularVelocity = steeringTorque;
        rigidbodyComponent->SetAngularVelocity(angularVelocity);
    }
}

void CarMovementComponent::HandleSideDrag() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
    const glm::vec2 rightVector = owner->GetTransform().GetRightVector();

    const glm::vec2 linearVelocity2D = rigidbodyComponent->GetLinearVelocity();
    const glm::vec3 linearVelocityTransformed(linearVelocity2D.x, 0, linearVelocity2D.y);
    const glm::vec2 localVelocity = glm::inverse((glm::mat3(owner->GetTransform().GetLocalMatrix()))) * linearVelocityTransformed;

    float sideDragStrength = localVelocity.x * sideDrag * mlg::Time::GetFixedTimeStep();
    rigidbodyComponent->AddForce(rightVector * sideDragStrength);
}

void CarMovementComponent::CounterTorque() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
    const glm::vec3 upVector = owner->GetTransform().GetUpVector();

    const float rotationVelocity = rigidbodyComponent->GetAngularSpeed();

    if (glm::abs(rotationVelocity) < 0.1)
        return;

    const float torqueStrength = std::clamp(-rotationVelocity, -1.f, 1.f) * counterTorque;
    rigidbodyComponent->AddTorque(upVector.y * torqueStrength);
}
