#include "AI/TrafficMovementComponent.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/Time.h"
#include "Core/Math.h"

#include "Physics/Colliders/ColliderShapes.h"

using json = nlohmann::json;

void TrafficMovementComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>().lock();

    rigidbodyComponent->SetLinearDrag(10.f);
    rigidbodyComponent->SetAngularDrag(10.f);

    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>().lock();

    //TODO: Initialize input here
}

TrafficMovementComponent::TrafficMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                                                   const std::string& configPath)
        : Component(owner, name) {
    LoadParameters(configPath);
}

void TrafficMovementComponent::PhysicsUpdate() {
    //TODO: Add physics functions
}

void TrafficMovementComponent::Update() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();

    float angularSpeed = glm::degrees(rigidbodyComponent->GetAngularSpeed());
    float bodySkew = mlg::Math::Clamp(-angularSpeed / 10.f, -15.f, 15.f);

    staticMeshComponent->GetTransform().SetRotation({{0.f, 0.f, glm::radians(bodySkew)}});
}

void TrafficMovementComponent::AIUpdate() {

}

void TrafficMovementComponent::LoadParameters(const std::string& path = "res/config/cars/testing.json") {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    acceleration = parameters["acceleration"];
    maxSpeed = parameters["maxSpeed"];
    backwardMaxSpeed = parameters["backwardMaxSpeed"];
    engineHandling = parameters["engineHandling"];
    handling = parameters["handling"];

    rotationSpeed = parameters["rotationSpeed"];
    rotationRadius = parameters["rotationRadius"];
}

TrafficMovementComponent::~TrafficMovementComponent() = default;
