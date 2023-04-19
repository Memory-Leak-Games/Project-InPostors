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
    float acceleration = 2200;
    float maxSpeed = 15;
    float backwardMaxSpeed = 5;
    float engineHandling = 2000;
    float handling = 4000;

    //Rotation and drifting
    float rotationSpeed = 350;
    float rotationRadius = 5;
    float sideDrag = 50;
    float counterTorque = 20;

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
