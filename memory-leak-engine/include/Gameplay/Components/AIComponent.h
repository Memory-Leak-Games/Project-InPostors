#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class RigidbodyComponent;
    class SteeringBehavior;

    class AIComponent : Component {
    private:
        std::shared_ptr<RigidbodyComponent> rigidbodyComponent;
        std::shared_ptr<TrafficMovementComponent> trafficMovementComponent;
        std::shared_ptr<SteeringBehavior> steering;

        float maxForce;

    public:
        AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                    const std::string& configPath = "res/config/ai.json");
        ~AiComponent() override;

        void Start() override;
        void Update() override;

        void LoadParameters(const std::string& path);

        float GetMaxForce() { return maxForce };
    };

}// namespace mlg
