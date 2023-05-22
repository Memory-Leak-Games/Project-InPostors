#pragma once

#include "Gameplay/Component.h"

namespace mlg {

    class AIComponent : Component {
    private:
        std::shared_ptr<RigidbodyComponent> rigidbodyComponent;
        std::shared_ptr<SteeringBehavior> steering;

    public:
        AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                    const std::string& configPath = "res/config/ai.json");
        ~AiComponent() override;

        void Start() override;
        void Update() override;

        void LoadParameters(const std::string& path);
    };

}// namespace mlg
