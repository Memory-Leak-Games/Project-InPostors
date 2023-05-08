#include "Car/CarMovementComponent.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Entity.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/HID/Input.h"
#include "Core/Time.h"
#include "Core/Math.h"

#include "Physics/Colliders/ColliderShapes.h"

#include "Car/CarInput.h"

// todo: test
#include "Rendering/ParticleSystem.h"
#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Renderer.h"

using json = nlohmann::json;

void CarMovementComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>().lock();

    rigidbodyComponent->SetLinearDrag(10.f);
    rigidbodyComponent->SetAngularDrag(5.f);

    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>().lock();

    carInput = GetOwner().lock()->GetComponentByClass<CarInput>().lock();

    //todo: test
    auto particleMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/Particles/smokeFX.json");
    particleSystem = std::make_shared<mlg::ParticleSystem>(particleMaterial, 100);

    mlg::Renderer::GetInstance()->AddLateRenderable(particleSystem);
}

CarMovementComponent::CarMovementComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                                           const std::string& configPath)
        : Component(owner, name) {
    LoadParameters(configPath);
}

void CarMovementComponent::PhysicsUpdate() {
    HandleEngineAndBraking();
    HandleSteering();
    HandleSideDrag();
    CounterTorque();
}

using Random = effolkronium::random_static;

void CarMovementComponent::Update() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();

    // TODO: test
    static float timeAccumulator;
    timeAccumulator += mlg::Time::GetDeltaSeconds();

    if (timeAccumulator > 0.1)
    {
        mlg::ParticleProps particleProps {};
        particleProps.lifeTime = 0.5f;
        particleProps.position = owner->GetTransform().GetWorldPosition()
                - owner->GetTransform().GetForwardVector() * 1.f;
        particleProps.position += glm::vec3 {
            Random::get(-1.f, 1.f),
            1.f,
            Random::get(-1.f, 1.f)
        } * 0.2f;

        particleProps.beginSize = glm::vec2(0.35) * Random::get(0.5f, 1.f);
        particleProps.endSize = glm::vec2(0.05);
        particleProps.beginColor = glm::vec4(1.f);
        particleProps.endColor = glm::vec4(0.f);

        particleProps.beginVelocity = glm::vec3(Random::get(-1.f, 1.f)) * 4.f;
        particleProps.beginVelocity.y = 0.f;
        particleProps.endVelocity = glm::vec3(0.f);

        particleSystem->Emit(particleProps);

        timeAccumulator = 0.f;
    }

    particleSystem->OnUpdate();

    float angularSpeed = glm::degrees(rigidbodyComponent->GetAngularSpeed());
    float bodySkew = mlg::Math::Clamp(-angularSpeed / 10.f, -15.f, 15.f);

    staticMeshComponent->GetTransform().SetRotation({{0.f, 0.f, glm::radians(bodySkew)}});
}

void CarMovementComponent::HandleEngineAndBraking() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();

    const glm::vec2 movementInput = carInput->GetMovementInput();

    const glm::vec3 forwardVector = owner->GetTransform().GetForwardVector();
    glm::vec2 forwardVector2D;
    forwardVector2D.x = forwardVector.x;
    forwardVector2D.y = forwardVector.z;

    const glm::vec3 localVelocity = rigidbodyComponent->GetLocalVelocity();

    float targetAccelerationForce = acceleration * movementInput.y * mlg::Time::GetFixedTimeStep();

    // Select speed by acceleration direction
    float currentMaxSpeed = movementInput.y > 0.f ? maxSpeed : backwardMaxSpeed;

    // Simulate smaller force engine at max speed
    targetAccelerationForce = mlg::Math::Lerp(targetAccelerationForce, 0.f,
                                              std::abs(localVelocity.z) / currentMaxSpeed);

    // engine handling
    if (std::abs(movementInput.y) < std::numeric_limits<float>::epsilon())
        targetAccelerationForce =
                engineHandling * mlg::Math::Clamp(-1.f, 1.f, -localVelocity.z) * mlg::Time::GetFixedTimeStep();

    // handling when velocity direction != movement direction
    if (localVelocity.z * movementInput.y < 0.f) {
        targetAccelerationForce = -mlg::Math::Sat(localVelocity.z) * handling * mlg::Time::GetFixedTimeStep();
    }

    rigidbodyComponent->AddForce(targetAccelerationForce * forwardVector2D);
}

void CarMovementComponent::HandleSteering() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
    const glm::vec3 localVelocity = rigidbodyComponent->GetLocalVelocity();
    const glm::vec2 movementInput = -carInput->GetMovementInput();

    const float steeringSpeedFactor = std::clamp(localVelocity.z / rotationRadius, -1.f, 1.f);
    const float steeringSpeed = movementInput.x * rotationSpeed * steeringSpeedFactor * mlg::Time::GetFixedTimeStep();

    float angularVelocity = rigidbodyComponent->GetAngularSpeed();

    if (glm::abs(angularVelocity) < glm::abs(steeringSpeed))
        rigidbodyComponent->SetAngularVelocity(steeringSpeed);
}

void CarMovementComponent::HandleSideDrag() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
    const glm::vec3 rightVector = owner->GetTransform().GetRightVector();

    glm::vec2 rightVector2D;
    rightVector2D.x = rightVector.x;
    rightVector2D.y = rightVector.z;

    const glm::vec3 localVelocity = rigidbodyComponent->GetLocalVelocity();

    const float sideDragStrength = localVelocity.x * sideDrag * mlg::Time::GetFixedTimeStep();
    rigidbodyComponent->AddForce(rightVector2D * sideDragStrength);
}

void CarMovementComponent::CounterTorque() {
    std::shared_ptr<mlg::Entity> owner = GetOwner().lock();
    const float rotationVelocity = rigidbodyComponent->GetAngularSpeed();

    const float torqueStrength = std::clamp(-rotationVelocity, -1.f, 1.f) * counterTorque;
    rigidbodyComponent->AddTorque(torqueStrength);
}

void CarMovementComponent::LoadParameters(const std::string& path = "res/config/cars/testing.json") {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    acceleration = parameters["acceleration"];
    maxSpeed = parameters["maxSpeed"];
    backwardMaxSpeed = parameters["backwardMaxSpeed"];
    engineHandling = parameters["engineHandling"];
    handling = parameters["handling"];

    rotationSpeed = parameters["rotationSpeed"];
    rotationRadius = parameters["rotationRadius"];
    sideDrag = parameters["sideDrag"];
    counterTorque = parameters["counterTorque"];
}

CarMovementComponent::~CarMovementComponent() = default;

