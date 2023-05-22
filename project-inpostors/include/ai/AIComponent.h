#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class RigidbodyComponent;
    class StaticMeshComponent;
    class Entity;
}

class TrafficMovementComponent;
class SteeringBehaviors;

class AIComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::shared_ptr<mlg::StaticMeshComponent> staticMeshComponent;
    SteeringBehaviors* steering;

    //Driving
    float acceleration;
    float maxSpeed;
    float backwardMaxSpeed;
    float engineHandling;
    float handling;

    //Rotation
    float rotationSpeed;
    float rotationRadius;

    float maxForce;
    float mass = 1.f;

public:
    AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                const std::string& configPath = "res/config/cars/testing.json");
    ~AIComponent() override;

    void Start() override;
    void Update() override;
    void PhysicsUpdate() override;
    void AIUpdate() override;

    float GetMaxForce() { return maxForce; }
    float GetMaxSpeed() { return maxSpeed; }
    const glm::vec2& GetPosition();
    glm::vec2 GetLinearVelocity();

    void LoadParameters(const std::string& path);

private:
    void HandleEngineAndBraking();
    void HandleSteering();
};
