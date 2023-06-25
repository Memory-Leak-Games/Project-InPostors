#include "Animation/MonsterTruckAnimationComponent.h"

#include <fstream>
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

#include "Utils/BlueprintManager.h"

#include "Core/Math.h"
#include "Core/Time.h"

using json = nlohmann::json;

void MonsterTruckAnimationComponent::Start() {
    rigidbodyComponent = GetOwner().lock()->GetComponentByClass<mlg::RigidbodyComponent>().lock();
}

MonsterTruckAnimationComponent::MonsterTruckAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                                                               const std::vector<std::weak_ptr<mlg::StaticMeshComponent>>& animMeshes)
    : Component(owner, name), animMeshes(animMeshes) {
}

MonsterTruckAnimationComponent::~MonsterTruckAnimationComponent() = default;

void MonsterTruckAnimationComponent::Update() {
    Rotate();
}

void MonsterTruckAnimationComponent::Rotate() {
    float rotationAngle = (float) mlg::Time::GetSeconds() * rigidbodyComponent->GetLinearVelocity().length();
    float rotationSine = std::sin(rotationAngle / 2);
    float rotationCosine = std::cos(rotationAngle / 2);

    glm::quat quaternion = {rotationCosine,
                            animRotation.x * rotationSine,
                            animRotation.y * rotationSine,
                            animRotation.z * rotationSine};

    for (const auto& animMesh : animMeshes) {
        animMesh.lock()->GetTransform().SetRotation(quaternion);
    }
}
