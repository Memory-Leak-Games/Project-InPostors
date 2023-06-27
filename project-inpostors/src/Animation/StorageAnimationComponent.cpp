#include "Animation/StorageAnimationComponent.h"

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

#include "Buildings/Storage.h"

#include "Utils/BlueprintManager.h"

#include "Core/Math.h"
#include "Core/Time.h"
#include "Managers/TaskManager.h"
#include "Scenes/LevelScene.h"

using json = nlohmann::json;

void StorageAnimationComponent::Start() {
    staticMeshComponent = GetOwner().lock()->GetComponentByClass<mlg::StaticMeshComponent>().lock();
    meshScale = staticMeshComponent->GetTransform().GetScale();

    storage = dynamic_pointer_cast<Storage>(GetOwner().lock());

    mlg::Scene* scene = mlg::SceneManager::GetCurrentScene();
    LevelScene* levelScene = dynamic_cast<LevelScene*>(scene);

    levelScene->GetTaskManager()->OnTaskFinished.append(
            [this](const TaskData& taskData) {
                animate = true;
                time = 0.f;
            });
}

StorageAnimationComponent::StorageAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                                                     const std::string& configPath) : Component(owner, name) {
    LoadParameters(configPath);
}

StorageAnimationComponent::~StorageAnimationComponent() = default;

void StorageAnimationComponent::Update() {
    if (!animate)
        return;

    SPDLOG_INFO("Mesh scale: ({}, {}, {})", meshScale.x, meshScale.y, meshScale.z);
    if (time > glm::pi<float>()) {
        storage->GetTransform().SetScale(meshScale);
        animate = false;
        time = 0.f;
        return;
    }

    if (storage != nullptr) {
        time += mlg::Time::GetDeltaSeconds() * storageSpeed;

        glm::vec3 targetScale = meshScale;
        float factor = glm::sin(time) * storageAmplitude;
        targetScale += (meshScale * factor);

        storage->GetTransform().SetScale(targetScale);
    }
}

void StorageAnimationComponent::LoadParameters(const std::string& path = "res/config/anim.json") {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    storageSpeed = parameters["storageSpeed"];
    storageAmplitude = parameters["storageAmplitude"];
}
