#include "ai/AIComponent.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"

#include "ai/TrafficMovementComponent.h"

using json = nlohmann::json;

AIComponent::AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                         const std::string& configPath)
        : Component(owner, name) {
    LoadParameters(configPath);
}

AIComponent::~AIComponent() = default;

void AIComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>().lock();
    trafficMovementComponent = GetOwner().lock()->GetComponentByClass<TrafficMovementComponent>().lock();

//    maxForce = trafficMovementComponent.lock();
}

void AIComponent::Update() {
    //TODO: Update AI here
}

void AIComponent::LoadParameters(const std::string& path) {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    cohesionWeight = parameters["cohesionWeight"];
    separationWeight = parameters["separationWeight"];
    alignmentWeight = parameters["alignmentWeight"];
    seekWeight = parameters["seekWeight"];
    arriveWeight = parameters["arriveWeight"];
    followPathWeight = parameters["followPathWeight"];
}
