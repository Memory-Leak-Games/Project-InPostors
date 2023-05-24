#pragma once

#include "Gameplay/Component.h"
#include <memory>

namespace mlg {
    class RigidbodyComponent;
    class StaticMeshComponent;
    class Entity;
}

class SteeringBehaviors;

class AIComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::shared_ptr<mlg::StaticMeshComponent> staticMeshComponent;
    std::unique_ptr<SteeringBehaviors> steering;

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
                const std::string& configPath = "res/config/cars/traffic.json");
    ~AIComponent() override;

    void Start() override;
    void Update() override;
    void PhysicsUpdate() override;
    void AIUpdate() override;

    float GetMaxForce() const { return maxForce; }
    float GetMaxSpeed() const { return maxSpeed; }
    glm::vec2 GetPosition() const;
    glm::vec2 GetLinearVelocity() const;
    glm::vec3 GetLocalVelocity() const;
    SteeringBehaviors* GetSteering() const;

    void LoadParameters(const std::string& path);

private:
    void HandleEngineAndBraking();
    void HandleSteering();
};
