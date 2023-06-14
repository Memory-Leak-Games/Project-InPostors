#include "Buildings/Factory.h"

#include <fstream>
#include <spdlog/spdlog.h>

#include "Core/SceneManager/SceneManager.h"
#include "Core/Settings/SettingsManager.h"
#include "Core/TimerManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Managers/TaskManager.h"
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

#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/ProgressBar.h"
#include "Utils/EquipmentComponent.h"
#include "Utils/ProductManager.h"

using json = nlohmann::json;
using Random = effolkronium::random_static;

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

    GenerateUI(result);

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
            staticMeshJson["position"][1],
            staticMeshJson["position"][2],
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
    const Blueprint& blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);

    if (working || equipmentComponent->IsFull() ||
        equipmentComponent->Has(blueprint.GetOutput()))
        return;

    if (!blueprint.CheckBlueprint(*equipmentComponent))
        return;

    working = true;

    // Remove inputs from eq
    for (const auto& item : blueprint.GetInput()) {
        equipmentComponent->RequestProduct(item);
    }

    auto productionLambda = [this, blueprint]() {
        equipmentComponent->AddProduct(blueprint.GetOutput());
        working = false;
        createProductSound->Play(2.f);
        CheckBlueprintAndStartWorking();
    };

    produceTimerHandle = mlg::TimerManager::Get()->SetTimer(
            blueprint.GetTimeToProcess(),
            false,
            productionLambda);
}

void Factory::FinishTask() {
    for (auto& product : equipmentComponent->GetProducts()) {
        mlg::Scene* currentScene = mlg::SceneManager::GetCurrentScene();
        auto* levelScene = dynamic_cast<LevelScene*>(currentScene);

        if (levelScene->GetTaskManager()->FinishTask(product))
            break;
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

    createProductSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/create_product.wav");

    equipmentComponent->equipmentChanged.append([this]() {
        CheckBlueprintAndStartWorking();
    });
}

void Factory::Update() {

    if (blueprintId != "None") {
        auto blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);
        float produceElapsed = mlg::TimerManager::Get()->GetTimerElapsedTime(produceTimerHandle);
        float timeToProcess = BlueprintManager::Get()->GetBlueprint(blueprintId).GetTimeToProcess();
        float timeRate = produceElapsed / timeToProcess;

        if (uiProgress)
            uiProgress->percentage = 0.096 + timeRate * (0.887 - 0.096);
    }

#ifdef DEBUG
    if (mlg::SettingsManager::Get<bool>(mlg::SettingsType::Debug, "showFactoryInfo")) {
        if (factoryType == FactoryType::Storage) {
            ImGui::Begin("Factories");
            ImGui::Text("Storage: %s", GetName().c_str());
            ImGui::End();

            return;
        }

        auto blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);
        ImGui::Begin("Factories");
        ImGui::Text(
                "%s, %s, timeToProduce: %f, wholeTime: %f, output: %i",
                GetName().c_str(),
                equipmentComponent->ToString().c_str(),
                mlg::TimerManager::Get()->GetTimerRemainingTime(produceTimerHandle),
                mlg::TimerManager::Get()->GetTimerRate(produceTimerHandle),
                equipmentComponent->GetNumberOfProduct(blueprint.GetOutput()));
        ImGui::End();
    }
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

void Factory::GenerateUI(const std::shared_ptr<Factory>& result) {

    auto font =
            mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/terminus-bold.ttf");
    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/pin_material.json");

    result->uiPin = result->AddComponent<mlg::Image>("uiPin", material).lock();
    result->uiPin->SetBillboardTarget(result);
    result->uiPin->SetSize({82.f, 82.f});
    result->uiPin->SetPosition({0.f, 50.f});

    if(result->blueprintId != "None") {
        auto blueprint = BlueprintManager::Get()->GetBlueprint(result->GetBlueprintId());
        material = ProductManager::GetInstance()->GetProduct(blueprint.GetOutput()).icon;
    } else {
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/storage_material.json");
    }
    result->uiIcon = result->AddComponent<mlg::Image>("uiIcon", material).lock();
    result->uiIcon->SetBillboardTarget(result);
    result->uiIcon->SetSize({24.f, 24.f});
    result->uiIcon->SetPosition({0.f, 50.f});

    if(result->GetBlueprintId() != "None") {
        auto blueprint = BlueprintManager::Get()->GetBlueprint(result->GetBlueprintId());
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/progress_bar_material.json");
        result->uiProgress = result->AddComponent<mlg::ProgressBar>("uiProgress", material).lock();
        result->uiProgress->SetBillboardTarget(result);
        result->uiProgress->SetSize({82.f, 82.f});
        result->uiProgress->SetPosition({0.f, 50.f});
        result->uiProgress->percentage = 0.f;

        if (blueprint.GetInput().size() > 0) {
            const auto& required = blueprint.GetInput();

            material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/required_panel_material.json");
            auto temp = result->AddComponent<mlg::Image>("uiRequiredPanel", material).lock();
            temp->SetBillboardTarget(result);
            temp->SetSize({42.f, 16.f});
            temp->SetPosition({0.f, 30.f});

            glm::vec2 billboardPos = {-11.f * (required.size() - 1.f), 30.f};
            for(int i = 0; i < required.size(); i++) {

                material = ProductManager::GetInstance()->GetProduct(required[i]).icon;
                temp = result->AddComponent<mlg::Image>("uiRequired", material).lock();
                temp->SetBillboardTarget(result);
                temp->SetSize({14.f, 14.f});
                temp->SetPosition(billboardPos);

                billboardPos.x += 22.f;
            }
        }
    }
}
