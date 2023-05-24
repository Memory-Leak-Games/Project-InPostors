#include "Buildings/Factory.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Core/TimerManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "Rendering/Particles/ParticleSystem.h"

#include "FX/FXLibrary.h"
#include "Gameplay/Components/ParticleSystemComponent.h"
#include "SceneGraph/Transform.h"

#include "Physics/Colliders/Collider.h"

#include "Utils/Blueprint.h"
#include "Utils/BlueprintManager.h"

#include "Utils/EquipmentComponent.h"


using json = nlohmann::json;

Factory::Factory(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {}

Factory::~Factory() = default;

std::shared_ptr<Factory> Factory::Create(uint64_t id, const std::string& name, bool isStatic,
                                         mlg::Transform* parent, const std::string& configPath) {
    auto result = std::shared_ptr<Factory>(new Factory(id, name, isStatic, parent));

    std::ifstream configFile{configPath};
    json configJson = json::parse(configFile);

    result->AddMesh(configJson["static-mesh"]);
    auto mainRigidbody = result->AddComponent<mlg::RigidbodyComponent>("MainRigidbody").lock();

    result->blueprintId = configJson["blueprintID"];

    const int equipmentSize = configJson["equipmentSize"];
    result->equipmentComponent = result->AddComponent<EquipmentComponent>("Equipment", equipmentSize).lock();


    for (const auto& colliderJson : configJson["colliders"]) {
        result->AddCollider(colliderJson, mainRigidbody.get());
    }

    for (const auto& emitterJson : configJson["emitters"]) {
        result->AddEmitter(emitterJson);
    }

    result->factoryType = magic_enum::enum_cast<FactoryType>(configJson["type"].get<std::string>()).value();
    if (result->factoryType == FactoryType::OneInput || result->factoryType == FactoryType::SeparateInputOutput) {
        result->AddTrigger(configJson["input"], "input", mainRigidbody.get());
    }

    if (result->factoryType == FactoryType::OneOutput || result->factoryType == FactoryType::SeparateInputOutput) {
        result->AddTrigger(configJson["output"], "output", mainRigidbody.get());
    }

    if (result->factoryType == FactoryType::OneInputOutput) {
        result->AddTrigger(configJson["input"], "inputOutput", mainRigidbody.get());
    }

    mainRigidbody->SetKinematic(true);
    return result;
}

void Factory::AddMesh(const json& staticMeshJson) {
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(staticMeshJson["model"].get<std::string>());
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(staticMeshJson["material"].get<std::string>());

    auto staticMeshComponent = AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);
    staticMeshComponent.lock()->GetTransform().SetPosition({
            staticMeshJson["position"][0],
            staticMeshJson["position"][1],
            staticMeshJson["position"][2],
    });

    staticMeshComponent.lock()->GetTransform().SetRotation(glm::radians(glm::vec3{
            staticMeshJson["rotation"][0],
            staticMeshJson["rotation"][1],
            staticMeshJson["rotation"][2],
    }));

    staticMeshComponent.lock()->GetTransform().SetRotation(glm::radians(glm::vec3{
            staticMeshJson["scale"]}));
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
    emitterComponent.lock()->GetTransform().SetPosition({
            emitterJson["position"][0],
            emitterJson["position"][1],
            emitterJson["position"][2],
    });
}

void Factory::AddTrigger(const json& triggerJson, const std::string& triggerName,
                         mlg::RigidbodyComponent* rigidbodyComponent) {

    glm::vec2 offset{triggerJson["offset"][0], triggerJson["offset"][1]};
    glm::vec2 size{triggerJson["size"][0], triggerJson["size"][1]};

    auto collider = rigidbodyComponent->AddTrigger<mlg::ColliderShape::Rectangle>(offset, size);
    collider.lock()->SetTag(triggerName);
}

void Factory::CheckBlueprintAndStartWorking() {
    // If factory is producing do not add another timer
    if (mlg::TimerManager::Get()->IsTimerValid(produceTimerHandle) || equipmentComponent->CheckIsFull())
        return;

    const Blueprint& blueprint = BlueprintManager::GetInstance()->GetBlueprint(blueprintId);

    if (!blueprint.CheckBlueprint(*equipmentComponent))
        return;

    auto productionLambda = [this, blueprint]() {
        equipmentComponent->AddProduct(blueprint.GetOutput());
        CheckBlueprintAndStartWorking();
    };

    produceTimerHandle = mlg::TimerManager::Get()->SetTimer(blueprint.GetTimeToProcess(), false, productionLambda);
}

void Factory::Start() {
    CheckBlueprintAndStartWorking();

    equipmentComponent->equipmentChanged.append([this]() {
        CheckBlueprintAndStartWorking();
    });
}

void Factory::Update() {
#ifdef DEBUG
    ImGui::Begin("Factories");
    ImGui::Text("%s, %s, timeToProduce: %f", GetName().c_str(), equipmentComponent->ToString().c_str(),
                mlg::TimerManager::Get()->GetTimerRemainingTime(produceTimerHandle));
    ImGui::End();
#endif
}

bool Factory::IsWorking() const {
    return mlg::TimerManager::Get()->IsTimerValid(produceTimerHandle);
}

const std::shared_ptr<EquipmentComponent>& Factory::GetEquipmentComponent() const {
    return equipmentComponent;
}

std::string Factory::GetBlueprintId() const { return blueprintId; }
