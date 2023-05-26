#include "ai/AIComponent.h"

#include <fstream>
#include <glm/fwd.hpp>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <nlohmann/json.hpp>

#include "Core/RGBA.h"
#include "Core/Settings/SettingsManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Entity.h"

#include "Rendering/Gizmos/Gizmos.h"
#include "ai/SteeringBehaviors.h"

#include "Core/Math.h"
#include "Core/Time.h"

#include "Physics/Colliders/ColliderShapes.h"

using json = nlohmann::json;

void AIComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>().lock();
    rigidbodyComponent->SetLinearDrag(10.f);
    rigidbodyComponent->SetAngularDrag(100.f);

    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>().lock();
}

AIComponent::AIComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                         const std::string& configPath)
    : Component(owner, name) {
    LoadParameters(configPath);
    steering = std::make_unique<SteeringBehaviors>(this);
}

AIComponent::~AIComponent() = default;

void AIComponent::Update() {
}

void AIComponent::PhysicsUpdate() {
    glm::vec2 steeringForce = steering->Calculate();
    rigidbodyComponent->AddForce(steeringForce);

    const glm::vec2 velocityDirection = mlg::Math::SafeNormal(rigidbodyComponent->GetLinearVelocity());
    if (glm::length(velocityDirection) > 0.f) {
        float angle = -std::atan2(velocityDirection.y, velocityDirection.x) - glm::half_pi<float>();
        rigidbodyComponent->SetRotation(angle);
    }

#ifdef DEBUG
    if (mlg::SettingsManager::Get<bool>(mlg::SettingsType::Debug, "showAiForces")) {
        glm::vec3 steeringForce3D = {steeringForce.x, 0.f, steeringForce.y};
        glm::vec3 position = GetOwner().lock()->GetTransform().GetPosition();
        mlg::Gizmos::DrawLine(position, position + steeringForce3D, mlg::RGBA::magenta);
    }
#endif
}

void AIComponent::AIUpdate() {
    //TODO: Update AI here
}

void AIComponent::LoadParameters(const std::string& path = "res/config/cars/traffic.json") {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    acceleration = parameters["acceleration"];
    maxForce = acceleration * mass;

    maxSpeed = parameters["maxSpeed"];
    backwardMaxSpeed = parameters["backwardMaxSpeed"];
    engineHandling = parameters["engineHandling"];
    handling = parameters["handling"];

    rotationSpeed = parameters["rotationSpeed"];
    rotationRadius = parameters["rotationRadius"];
}

glm::vec2 AIComponent::GetPosition() const {
    return rigidbodyComponent->GetPosition();
}

glm::vec2 AIComponent::GetLinearVelocity() const {
    return rigidbodyComponent->GetLinearVelocity();
}

glm::vec3 AIComponent::GetLocalVelocity() const {
    return rigidbodyComponent->GetLocalVelocity();
}

SteeringBehaviors* AIComponent::GetSteering() const {
    return steering.get();
}