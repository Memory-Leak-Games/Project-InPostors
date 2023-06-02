#include "Buildings/Factory.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#include "Core/SceneManager/SceneManager.h"
#include "Core/TimerManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Gameplay/TaskManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "Rendering/Particles/ParticleSystem.h"

#include "FX/FXLibrary.h"
#include "Gameplay/Components/ParticleSystemComponent.h"
#include "SceneGraph/Transform.h"

#include "Physics/Colliders/Collider.h"

#include "Scenes/LevelScene.h"
#include "Utils/Blueprint.h"
#include "Utils/BlueprintManager.h"

#include "Utils/EquipmentComponent.h"


using json = nlohmann::json;

Factory::Factory(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {}

Factory::~Factory() = default;

std::shared_ptr<Factory> Factory::Create(uint64_t id, const std::string& name, bool isStatic,
                                         mlg::Transform* parent, const std::string& configPath) {
    auto result =
            std::shared_ptr<Factory>(new Factory(id, name, isStatic, parent));

    std::ifstream configFile{configPath};
    json configJson = json::parse(configFile);

    std::string factoryType = configJson["type"];
    result->factoryType = magic_enum::enum_cast<FactoryType>(factoryType).value();

    result->AddMesh(configJson["static-mesh"]);
    result->mainRigidbody = result->AddComponent<mlg::RigidbodyComponent>("MainRigidbody").lock();

    result->blueprintId = configJson.value("blueprintID", "None");

    const int equipmentSize = configJson.value("equipmentSize", 256);
    result->equipmentComponent =
            result->AddComponent<EquipmentComponent>("Equipment", equipmentSize).lock();


    for (const auto& colliderJson : configJson["colliders"]) {
        result->AddCollider(colliderJson, result->mainRigidbody.get());
    }

    for (const auto& emitterJson : configJson["emitters"]) {
        result->AddEmitter(emitterJson);
    }

    result->AddTriggers(configJson);
    result->mainRigidbody->SetKinematic(true);
    return result;
}

void Factory::AddMesh(const json& staticMeshJson) {
    auto model =
            mlg::AssetManager::GetAsset<mlg::ModelAsset>(
                    staticMeshJson["model"]);
    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    staticMeshJson["material"].get<std::string>());

    auto staticMeshComponent =
            AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);

    staticMeshComponent.lock()->GetTransform().SetPosition({
            staticMeshJson["position"][0],
            0.0,
            staticMeshJson["position"][1],
    });

    meshOffset = {staticMeshJson["position"][0], staticMeshJson["position"][1]};

    staticMeshComponent.lock()->GetTransform().SetRotation(glm::radians(glm::vec3{
            0.f,
            staticMeshJson["rotation"],
            0.f,
    }));

    staticMeshComponent.lock()->GetTransform().SetScale(glm::vec3{
            staticMeshJson["scale"]});
}

void Factory::AddCollider(const json& colliderJson, mlg::RigidbodyComponent* rigidbodyComponent) {
    glm::vec2 offset{colliderJson["offset"][0], colliderJson["offset"][1]};
    glm::vec2 size{colliderJson["size"][0], colliderJson["size"][1]};

    rigidbodyComponent->AddCollider<mlg::ColliderShape::Rectangle>(offset, size);
}

void Factory::AddEmitter(const json& emitterJson) {
    const std::string id = emitterJson["id"].get<std::string>();
    std::shared_ptr<mlg::ParticleSystem> emitter = FXLibrary::Get(id);
    auto emitterComponent = AddComponent<mlg::ParticleSystemComponent>(id, emitter);
    glm::vec3 emitterPosition = {
            emitterJson["position"][0],
            emitterJson["position"][1],
            emitterJson["position"][2]};
    emitterPosition.x += meshOffset.x;
    emitterPosition.z += meshOffset.y;
    emitterComponent.lock()->GetTransform().SetPosition(emitterPosition);
}

void Factory::AddTriggers(const nlohmann::json& configJson) {
    if (factoryType == FactoryType::OneInput ||
        factoryType == FactoryType::SeparateInputOutput ||
        factoryType == FactoryType::Storage) {
        AddTrigger(
                configJson["input"],
                "input",
                mainRigidbody.get());
    }

    if (factoryType == FactoryType::OneOutput ||
        factoryType == FactoryType::SeparateInputOutput) {
        AddTrigger(
                configJson["output"],
                "output",
                mainRigidbody.get());
    }

    if (factoryType == FactoryType::OneInputOutput) {
        AddTrigger(
                configJson["input"],
                "inputOutput",
                mainRigidbody.get());
    }
}

void Factory::AddTrigger(const json& triggerJson, const std::string& triggerName,
                         mlg::RigidbodyComponent* rigidbodyComponent) {

    glm::vec2 offset{triggerJson["offset"][0], triggerJson["offset"][1]};
    glm::vec2 size{triggerJson["size"][0], triggerJson["size"][1]};

    auto collider = rigidbodyComponent->AddTrigger<mlg::ColliderShape::Rectangle>(offset, size);
    collider.lock()->SetTag(triggerName);
}

void Factory::CheckBlueprintAndStartWorking() {
    if (working || equipmentComponent->IsFull())
        return;

    const Blueprint& blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);

    if (!blueprint.CheckBlueprint(*equipmentComponent))
        return;

    working = true;

    // Remove inputs from eq
    for (const auto& item : blueprint.GetInput()) {
        equipmentComponent->RequestProduct(item);
        SPDLOG_DEBUG("Requested {}", item);
    }

    auto productionLambda = [this, blueprint]() {
        equipmentComponent->AddProduct(blueprint.GetOutput());
        working = false;
        CheckBlueprintAndStartWorking();
    };

    produceTimerHandle = mlg::TimerManager::Get()->SetTimer(blueprint.GetTimeToProcess(), false, productionLambda);
}

void Factory::FinishTask() {
    for (auto& product : equipmentComponent->GetProducts()) {
        mlg::Scene* currentScene = mlg::SceneManager::GetCurrentScene();
        auto* levelScene = dynamic_cast<LevelScene*>(currentScene);

        levelScene->GetTaskManager()->FinishTask(product);
    }
}

void Factory::Start() {
    if (factoryType == FactoryType::Storage) {
        equipmentComponent->equipmentChanged.append([this]() {
            FinishTask();
        });
        return;
    }

    CheckBlueprintAndStartWorking();

    equipmentComponent->equipmentChanged.append([this]() {
        CheckBlueprintAndStartWorking();
    });
}

void Factory::Update() {
#ifdef DEBUG
    if (factoryType == FactoryType::Storage) {
        ImGui::Begin("Factories");
        ImGui::Text("Storage: %s", GetName().c_str());
        ImGui::End();

        return;
    }

    auto blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);

    ImGui::Begin("Factories");
    ImGui::Text("%s, %s, timeToProduce: %f, output: %i",
                GetName().c_str(),
                equipmentComponent->ToString().c_str(),
                mlg::TimerManager::Get()->GetTimerRemainingTime(produceTimerHandle),
                equipmentComponent->GetNumberOfProduct(blueprint.GetOutput()));
    ImGui::End();
#endif
}

bool Factory::IsWorking() const {
    return mlg::TimerManager::Get()->IsTimerValid(produceTimerHandle);
}

const std::vector<std::string> Factory::GetInputs() const {
    std::vector<std::string> result;

    if (factoryType != FactoryType::Storage) {
        result = BlueprintManager::Get()->GetBlueprint(blueprintId).GetInput();
        return result;
    }

    mlg::Scene* currentScene = mlg::SceneManager::GetCurrentScene();
    auto* levelScene = dynamic_cast<LevelScene*>(currentScene);

    std::vector<TaskData> tasks = levelScene->GetTaskManager()->GetActiveTasks();
    for (const auto& task : tasks) {
        result.push_back(task.productId);
    }

    return result;
}

const std::shared_ptr<EquipmentComponent>& Factory::GetEquipmentComponent() const {
    return equipmentComponent;
}

std::string Factory::GetBlueprintId() const { return blueprintId; }
