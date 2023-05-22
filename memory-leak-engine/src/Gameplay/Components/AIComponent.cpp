#include "Gameplay/Components/AIComponent.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "AI/TrafficMovementComponent"

using json = nlohmann::json;

namespace mlg {

    AIComponent::AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                             const std::string& configPath = "res/config/ai.json")
            : Component(owner, name) {
        LoadParameters(configPath);
    }

    AIComponent::~AIComponent() = default;

    void AIComponent::Start() {
        rigidbodyComponent = GetOwner().lock().GetComponentByClass<RigidbodyComponent>().lock();
        trafficMovementComponent = GetOwner().lock.GetComponentByClass<TrafficMovementComponent>().lock();

        maxForce = trafficMovementComponent.lock().
    }

    void AIComponent::Update() {
        //TODO: Update AI here
    }
}// namespace mlg