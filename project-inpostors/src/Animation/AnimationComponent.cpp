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
}

AnimationComponent::AnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                                       const std::string& configPath) : Component(owner, name) {
    LoadParameters(configPath);

    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>().lock();
    meshScale = staticMeshComponent->GetTransform().GetScale();

    factory = dynamic_pointer_cast<Factory>(GetOwner().lock());
}

AnimationComponent::~AnimationComponent() = default;

void AnimationComponent::Update() {
    if (factory != nullptr) {
        if (outputAnimOn && factory->factoryEquipment->IsOutputPresent()) {
            glm::vec3 targetScale = meshScale;

            float factor = glm::sin(mlg::Time::GetSeconds() * outputAnimSpeed) * outputAnimWeight;
            targetScale += (meshScale * factor);

            staticMeshComponent->GetTransform().SetScale(targetScale);
            return;
        }

        if (workingAnimOn && factory->IsWorking()) {
            glm::vec3 targetScale = meshScale;

            float factor = glm::sin(mlg::Time::GetSeconds() * workingAnimSpeed) * workingAnimWeight;
            targetScale.y += (meshScale.y * factor);

            staticMeshComponent->GetTransform().SetScale(targetScale);
            return;
        }
    }
}

void AnimationComponent::LoadParameters(const std::string& path = "res/config/anim.json") {
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
