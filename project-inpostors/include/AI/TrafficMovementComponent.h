#pragma onceonce

#include "Gameplay/Component.h"

namespace mlg {
    class RigidbodyComponent;
    class StaticMeshComponent;
    class Entity;
}

class TrafficMovementComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::shared_ptr<mlg::StaticMeshComponent> staticMeshComponent;

    //Driving
    float acceleration;
    float maxSpeed;
    float backwardMaxSpeed;
    float engineHandling;
    float handling;

    //Rotation
    float rotationSpeed;
    float rotationRadius;

public:
    TrafficMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                             const std::string& configPath = "res/config/cars/testing.json");
    ~TrafficMovementComponent() override;

    void Start() override;

    void Update() override;
    void PhysicsUpdate() override;
    void AIUpdate() override;

    void LoadParameters(const std::string& path);

private:
    //TODO: functions handling movement
};
