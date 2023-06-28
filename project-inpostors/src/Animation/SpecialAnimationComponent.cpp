#include "Animation/SpecialAnimationComponent.h"

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

void SpecialAnimationComponent::Start() {
    jumpPosition = animMesh.lock()->GetTransform().GetPosition();
}

SpecialAnimationComponent::SpecialAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                                                     const std::weak_ptr<mlg::StaticMeshComponent>& animMesh,
                                                     const AnimationType animType)
    : Component(owner, name), animMesh(animMesh), animType(animType) {
}

SpecialAnimationComponent::~SpecialAnimationComponent() = default;

void SpecialAnimationComponent::Update() {
    if (animType == rotateZ) {
        Rotate(wheelRotation, wheelFactor);
    } else if (animType == rotateYZ) {
        Rotate(donutRotation, donutFactor);
    } else if (animType == jump) {
        Jump();
        Rotate(jumpRotation, jumpFactor);
    }
}

void SpecialAnimationComponent::Rotate(glm::vec3 rotationVector, float factor) {
    auto rotationAngle = (float) mlg::Time::GetSeconds() / factor;
    float rotationSine = std::sin(rotationAngle / 2);
    float rotationCosine = std::cos(rotationAngle / 2);

    glm::quat quaternion = {rotationCosine,
                            rotationVector.x * rotationSine,
                            rotationVector.y * rotationSine,
                            rotationVector.z * rotationSine};

    animMesh.lock()->GetTransform().SetRotation(quaternion);
}

void SpecialAnimationComponent::Jump() {
    jumpPosition.y = std::sin(mlg::Time::GetSeconds()) + 1;
    animMesh.lock()->GetTransform().SetPosition(jumpPosition);
}
