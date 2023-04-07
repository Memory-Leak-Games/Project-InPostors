#pragma once

#include "Gameplay/Component.h"

#include "Physics/Colliders/ColliderShapes.h"
#include "Physics/Rigidbody.h"

namespace mlg {

    class RigidbodyComponent : public Component {
    private:
        std::shared_ptr<Rigidbody> rigidbody;

    public:
        RigidbodyComponent(const std::weak_ptr<Entity>& owner, const std::string& name);

        void Start() override;

        void PhysicsUpdate() override;
        void Update() override;

        void AddForce(glm::vec2 force);
        void AddForce(glm::vec2 force, glm::vec2 localPosition);
        void AddTorque(float value);

        void SetLinearDrag(float value);
        void SetAngularDrag(float value);

        template<typename T, typename ... Args>
        void AddCollider(Args&& ... args) {
            auto shape = std::make_unique<T>(glm::vec2(0.f), std::forward<Args>(args) ...);
            rigidbody->AddCollider(std::move(shape));
        }

        void SetKinematic(bool isKinematic);

        ~RigidbodyComponent() override;
    };

} // mlg
