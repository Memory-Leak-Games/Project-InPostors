#include "ai/AIComponent.h"

#include <fstream>
#include <glm/fwd.hpp>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <nlohmann/json.hpp>

#include "Core/RGBA.h"
#include "Core/SceneManager/Scene.h"
#include "Core/SceneManager/SceneManager.h"
#include "Core/Settings/SettingsManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Entity.h"

#include "Rendering/Gizmos/Gizmos.h"
#include "Scenes/LevelScene.h"
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

    mlg::Scene* currentScene = mlg::SceneManager::GetCurrentScene();
    auto* levelScene = dynamic_cast<LevelScene*>(currentScene);
    steering->SetNavigationGraph(levelScene->GetNavigationGraph());

    steering->CreatePath(256);

    GetSteering()->TrafficDriveOn();
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
    if (steering->GetNavigationGraph() != nullptr) {
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
            glm::vec3 nextWaypoint = {steering->GetCurrentWaypoint().x, 0.f, steering->GetCurrentWaypoint().y};
            glm::vec3 position = GetOwner().lock()->GetTransform().GetPosition();
            mlg::Gizmos::DrawLine(position, position + steeringForce3D, mlg::RGBA::magenta);
            mlg::Gizmos::DrawLine(position, nextWaypoint, mlg::RGBA::red);
        }
#endif
    }
}

void AIComponent::AIUpdate() {
}

void AIComponent::LoadParameters(const std::string& path = "res/config/cars/traffic.json") {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    acceleration = parameters["acceleration"];
    maxForce = acceleration * mass;

    maxSpeed = parameters["maxSpeed"];
    viewDistance = parameters["viewDistance"];
}

float AIComponent::GetMaxForce() const {
    return maxForce;
}

float AIComponent::GetMaxSpeed() const {
    return maxSpeed;
}

float AIComponent::GetViewDistance() const {
    return viewDistance;
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

void AIComponent::SetNavigationGraph(std::shared_ptr<NavigationGraph> navGraph) {
    steering->SetNavigationGraph(navGraph);
}
