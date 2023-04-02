#pragma once

#include "Gameplay/Component.h"

namespace mlg {

    class RigidbodyComponent : public Component {
        std::shared_ptr<class PhysicsState> physicsState;

    public:
        RigidbodyComponent(const std::weak_ptr<Entity>& owner, const std::string& name);

        void PhysicsUpdate() override;

        void AddForce(glm::vec2 force);
        void AddForce(glm::vec2 force, glm::vec2 localPosition);
        void AddTorque(float value);

        void SetLinearDrag(float value);
        void SetAngularDrag(float value);

        ~RigidbodyComponent() override;
    };

} // mlg
