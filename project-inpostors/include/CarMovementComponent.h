#pragma once

#include <Gameplay/Component.h>

namespace mlg {
    class RigidbodyComponent;
    class StaticMeshComponent;
    class Entity;
}

class CarMovementComponent : public mlg::Component {
private:
    std::weak_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::weak_ptr<mlg::StaticMeshComponent> staticMeshComponent;
public:
    CarMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name);

    void Start() override;

    void Update() override;

    void PhysicsUpdate() override;
};
