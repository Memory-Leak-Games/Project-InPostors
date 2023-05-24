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
}

void AIComponent::PhysicsUpdate() {
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

    rigidbodyComponent->AddForce(steeringForce);
}

void AIComponent::HandleSteering() {
    const glm::vec2 steeringForce = steering->Calculate();

    glm::vec2 normalVector = mlg::Math::SafeNormal(steeringForce);
    float rotation = std::atan2(normalVector.y, normalVector.x);

    SPDLOG_INFO("Angle: {}", rotation);

    rigidbodyComponent->SetRotation(rotation);
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
