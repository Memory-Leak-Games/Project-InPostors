#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class RigidbodyComponent;
    class SteeringBehaviors;
    class Entity;
}

class TrafficMovementComponent;

class AIComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::shared_ptr<TrafficMovementComponent> trafficMovementComponent;
    std::shared_ptr<mlg::SteeringBehaviors> steering;

    // Wights for behaviors
    float cohesionWeight;
    float separationWeight;
    float alignmentWeight;
    float seekWeight;
    float arriveWeight;
    float followPathWeight;

public:
    AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                const std::string& configPath = "res/config/ai.json");
    ~AIComponent() override;

    void Start() override;
    void Update() override;

    void LoadParameters(const std::string& path);
};
