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
    float engineHandling = 2000;
    float handling = 4000;

    //Rotation and drifting
    float rotationSpeed = 600;
    float rotationRadius = 3;
    float sideDrag = 20;
    float counterTorque = 5;

    float forward;
    float right;

public:
    CarMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name);

    void Start() override;

    void Update() override;

    void PhysicsUpdate() override;

    void HandleEngineAndBraking();
    void HandleSteering();
    void HandleSideDrag();
    void CounterTorque();

};
