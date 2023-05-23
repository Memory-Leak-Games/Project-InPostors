#include "ai/AIComponent.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "ai/SteeringBehaviors.h"

#include "Core/Time.h"
#include "Core/Math.h"

#include "Physics/Colliders/ColliderShapes.h"

using json = nlohmann::json;

void AIComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>().lock();

    rigidbodyComponent->SetLinearDrag(10.f);
    rigidbodyComponent->SetAngularDrag(10.f);

    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>().lock();
}

AIComponent::AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                         const std::string& configPath)
        : Component(owner, name) {
    LoadParameters(configPath);
    steering = new SteeringBehaviors(this);
}

AIComponent::~AIComponent() = default;

void AIComponent::Update() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();

//    float angularSpeed = glm::degrees(rigidbodyComponent->GetAngularSpeed());
//    float bodySkew = mlg::Math::Clamp(-angularSpeed / 10.f, -15.f, 15.f);
//
//    staticMeshComponent->GetTransform().SetRotation({{0.f, 0.f, glm::radians(bodySkew)}});
}

void AIComponent::PhysicsUpdate() {
//    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
//    const glm::vec3 forwardVector = owner->GetTransform().GetForwardVector();
//    glm::vec2 forwardVector2D;
//    forwardVector2D.x = forwardVector.x;
//    forwardVector2D.y = forwardVector.z;
//
//    glm::vec2 steeringForce;
//    steeringForce = steering->Calculate();
//
//    acceleration = steeringForce / mass;

    HandleEngineAndBraking();
    HandleSteering();
}

void AIComponent::HandleEngineAndBraking() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
    const glm::vec3 forwardVector = owner->GetTransform().GetForwardVector();
    glm::vec2 forwardVector2D;
    forwardVector2D.x = forwardVector.x;
    forwardVector2D.y = forwardVector.z;

    const glm::vec3 localVelocity = rigidbodyComponent->GetLocalVelocity();

    glm::vec2 steeringForce = steering->Calculate();

    float targetAccelerationForce = acceleration * steeringForce.y * mlg::Time::GetFixedTimeStep();

    // Select speed by acceleration direction
    float currentMaxSpeed = steeringForce.y > 0.f ? maxSpeed : backwardMaxSpeed;

    // Simulate smaller force engine at max speed
    targetAccelerationForce = mlg::Math::Lerp(targetAccelerationForce, 0.f,
                                              std::abs(localVelocity.z) / currentMaxSpeed);

    // engine handling
    if (std::abs(steeringForce.y) < std::numeric_limits<float>::epsilon())
        targetAccelerationForce =
                engineHandling * mlg::Math::Clamp(-1.f, 1.f, -localVelocity.z) * mlg::Time::GetFixedTimeStep();

    // handling when velocity direction != movement direction
    if (localVelocity.z * steeringForce.y < 0.f) {
        targetAccelerationForce = -mlg::Math::Sat(localVelocity.z) * handling * mlg::Time::GetFixedTimeStep();
    }

    rigidbodyComponent->AddForce(targetAccelerationForce * forwardVector2D);
}

void AIComponent::HandleSteering() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
    const glm::vec3 localVelocity = rigidbodyComponent->GetLocalVelocity();
    const glm::vec2 steeringForce = steering->Calculate();

    const float steeringSpeedFactor = std::clamp(localVelocity.z / rotationRadius, -1.f, 1.f);
    const float steeringSpeed = steeringForce.x * rotationSpeed * steeringSpeedFactor * mlg::Time::GetFixedTimeStep();

    float angularVelocity = rigidbodyComponent->GetAngularSpeed();

    if (glm::abs(angularVelocity) < glm::abs(steeringSpeed))
        rigidbodyComponent->SetAngularVelocity(steeringSpeed);
}

void AIComponent::AIUpdate() {
    //TODO: Update AI here
}

void AIComponent::LoadParameters(const std::string& path = "res/config/cars/testing.json") {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    acceleration = parameters["acceleration"];
    maxForce = acceleration * mass;

    maxSpeed = parameters["maxSpeed"];
    backwardMaxSpeed = parameters["backwardMaxSpeed"];
    engineHandling = parameters["engineHandling"];
    handling = parameters["handling"];

    rotationSpeed = parameters["rotationSpeed"];
    rotationRadius = parameters["rotationRadius"];
}

const glm::vec2 &AIComponent::GetPosition() {
    return rigidbodyComponent->GetPosition();
}

glm::vec2 AIComponent::GetLinearVelocity() {
    return rigidbodyComponent->GetLinearVelocity();
}

glm::vec3 AIComponent::GetLocalVelocity() {
    return rigidbodyComponent->GetLocalVelocity();
}

SteeringBehaviors* AIComponent::GetSteering() {
    return steering;
}
