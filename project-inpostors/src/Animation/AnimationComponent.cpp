#include "Animation/AnimationComponent.h"

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

#include "Buildings/Factory.h"
#include "Buildings/FactoryEquipmentComponent.h"

#include "Utils/BlueprintManager.h"

#include "Core/Math.h"
#include "Core/Time.h"

using json = nlohmann::json;

void AnimationComponent::Start() {
    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>().lock();
    meshScale = staticMeshComponent->GetTransform().GetScale();

    factory = GetOwner().lock()->GetComponentByClass<FactoryEquipmentComponent>().lock();
}

AnimationComponent::AnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                                       const std::string& configPath) : Component(owner, name) {
    LoadParameters(configPath);
}

AnimationComponent::~AnimationComponent() = default;

void AnimationComponent::Update() {
    if (factory != nullptr) {
        if (factory->IsOutputPresent()) {
            glm::vec3 targetScale = meshScale;

            float factor = glm::sin(mlg::Time::GetSeconds() * animSpeed) * wiggleWeight;
            targetScale += (meshScale * factor);

            staticMeshComponent->GetTransform().SetScale(targetScale);
        }
    }
}

void AnimationComponent::LoadParameters(const std::string& path = "res/config/anim.json") {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    animSpeed = parameters["animSpeed"];
    wiggleWeight = parameters["wiggleWeight"];
}
