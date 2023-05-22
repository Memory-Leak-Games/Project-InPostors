#include "ai/AIComponent.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"

#include "ai/TrafficMovementComponent.h"

using json = nlohmann::json;

AIComponent::AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                         const std::string& configPath = "res/config/ai.json")
        : Component(owner, name) {
    LoadParameters(configPath);
}

AIComponent::~AIComponent() = default;

void AIComponent::Start() {
    rigidbodyComponent = GetOwner().lock().GetComponentByClass<mlg::RigidbodyComponent>().lock();
    trafficMovementComponent = GetOwner().lock.GetComponentByClass<TrafficMovementComponent>().lock();

//    maxForce = trafficMovementComponent.lock();
}

void AIComponent::Update() {
    //TODO: Update AI here
}
