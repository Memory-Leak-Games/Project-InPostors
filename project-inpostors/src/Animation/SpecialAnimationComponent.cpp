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
}

SpecialAnimationComponent::SpecialAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                                                     const std::weak_ptr<mlg::StaticMeshComponent>& animMesh,
                                                     const AnimationType animType,
                                                     const std::string& configPath)
    : Component(owner, name), animMesh(animMesh), animType(animType) {
    LoadParameters(configPath);
}

SpecialAnimationComponent::~SpecialAnimationComponent() = default;

void SpecialAnimationComponent::Update() {
    if (animType == rotateZ) {
        RotateZ();
    } else if (animType == rotateYZ) {
        RotateYZ();
    }
}

void SpecialAnimationComponent::LoadParameters(const std::string& path = "res/config/anim.json") {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    outputAnimOn = parameters["outputAnimOn"];
    outputAnimSpeed = parameters["outputAnimSpeed"];
    outputAnimWeight = parameters["outputAnimWeight"];

    workingAnimOn = parameters["workingAnimOn"];
    workingAnimSpeed = parameters["workingAnimSpeed"];
    workingAnimWeight = parameters["workingAnimWeight"];
}

void SpecialAnimationComponent::RotateZ() {
    auto rotationAngle = (float) mlg::Time::GetSeconds() / 4;
    float rotationSine = std::sin(rotationAngle / 2);
    float rotationCosine = std::cos(rotationAngle / 2);

    glm::quat quaternion = {rotationCosine,
                            zRotation.x * rotationSine,
                            zRotation.y * rotationSine,
                            zRotation.z * rotationSine};

    animMesh.lock()->GetTransform().SetRotation(quaternion);
}

void SpecialAnimationComponent::RotateYZ() {
    auto rotationAngle = (float) mlg::Time::GetSeconds() / 2;
    float rotationSine = std::sin(rotationAngle / 2);
    float rotationCosine = std::cos(rotationAngle / 2);

    glm::quat quaternion = {rotationCosine,
                            yzRotation.x * rotationSine,
                            yzRotation.y * rotationSine,
                            yzRotation.z * rotationSine};

    animMesh.lock()->GetTransform().SetRotation(quaternion);
}
