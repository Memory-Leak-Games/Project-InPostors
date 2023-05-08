#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class RigidbodyComponent;
    class StaticMeshComponent;
    class Entity;
}

class CarMovementComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::shared_ptr<mlg::StaticMeshComponent> staticMeshComponent;
    std::shared_ptr<class CarInput> carInput;

    //Driving
    float acceleration;
    float maxSpeed;
    float backwardMaxSpeed;
    float engineHandling;
    float handling;

    //Rotation and drifting
    float rotationSpeed;
    float rotationRadius;
    float sideDrag;
    float counterTorque;

public:
    CarMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                         const std::string& configPath = "res/config/cars/testing.json");
    ~CarMovementComponent() override;

    void Start() override;

    void Update() override;
    void PhysicsUpdate() override;

    void LoadParameters(const std::string& path);

private:
    void HandleEngineAndBraking();
    void HandleSteering();
    void HandleSideDrag();
    void CounterTorque();
};
