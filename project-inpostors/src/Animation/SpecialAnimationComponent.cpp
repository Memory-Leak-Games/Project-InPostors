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
                                                     const std::string& configPath) : Component(owner, name), animMesh(animMesh) {
    LoadParameters(configPath);
}

SpecialAnimationComponent::~SpecialAnimationComponent() = default;

void SpecialAnimationComponent::Update() {
    animMesh.lock()->GetTransform().SetScale({2.f, 2.f, 2.f});
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
