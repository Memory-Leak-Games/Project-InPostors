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

    //Driving
    float acceleration = 2400;
    float maxSpeed = 15;
    float backwardMaxSpeed = 10;
    float engineHandling = 100;
    float handling = 4000;

    //Rotation and drifting
    float rotationSpeed = 200;
    float rotationRadius = 3;
    float sideDrag = 40;
    float counterTorque = 5;

    float currentAccelerationForce;

    float forward;
    float right;

public:
    CarMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name);

    void Start() override;

    void Update() override;
    void PhysicsUpdate() override;

private:
    void HandleEngineAndBraking();
    void HandleSteering();
    void HandleSideDrag();
    void CounterTorque();
};
