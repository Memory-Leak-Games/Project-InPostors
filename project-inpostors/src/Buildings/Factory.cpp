#include "Buildings/Factory.h"

#include <fstream>
#include <spdlog/spdlog.h>
#include <vector>

#include "Buildings/InteractiveBuilding.h"
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

#include "Buildings/FactoryEquipmentComponent.h"
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
    : InteractiveBuilding(id, name, isStatic, parent) {}

Factory::~Factory() = default;

std::shared_ptr<Factory> Factory::Create(
        uint64_t id, const std::string& name, bool isStatic,
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

void Factory::AddTriggers(const nlohmann::json& configJson) {
    if (factoryType == FactoryType::OneInput ||
        factoryType == FactoryType::SeparateInputOutput) {
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

void Factory::CheckBlueprintAndStartWorking() {
    const Blueprint& blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);

    if (working || factoryEquipment->IsOutputPresent())
        return;

    if (!CheckBlueprint())
        return;

    working = true;

    // Remove inputs from eq
    factoryEquipment->ClearInput();

    auto productionLambda = [this, blueprint]() {
        working = false;
        createProductSound->Play(2.f);
        factoryEquipment->Produce();
    };

    produceTimerHandle = mlg::TimerManager::Get()->SetTimer(
            blueprint.GetTimeToProcess(),
            false,
            productionLambda);
}

void Factory::Start() {
    StartAsFactory();
}

void Factory::StartAsFactory() {
    createProductSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/create_product.wav");

    factoryEquipment = AddComponent<FactoryEquipmentComponent>(
                               "FactoryEquipmentComponent",
                               blueprintId)
                               .lock();

    CheckBlueprintAndStartWorking();

    factoryEquipment->inputProductAdded.append([this]() {
        CheckBlueprintAndStartWorking();
    });

    factoryEquipment->outputProductRemoved.append([this]() {
        CheckBlueprintAndStartWorking();
    });
}

void Factory::Update() {
    UpdateUi();
}

void Factory::UpdateUi() {
    if (blueprintId != "None") {
        auto blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);

        if (IsWorking()) {
            float produceElapsed = mlg::TimerManager::Get()->GetTimerElapsedTime(produceTimerHandle);
            float timeToProcess = BlueprintManager::Get()->GetBlueprint(blueprintId).GetTimeToProcess();
            float timeRate = produceElapsed / timeToProcess;
            uiProgress->percentage = timeRate * 0.75f;
        } else {
            uiProgress->percentage = factoryEquipment->IsOutputPresent();
        }
    }
}

bool Factory::IsWorking() const {
    return mlg::TimerManager::Get()->IsTimerValid(produceTimerHandle);
}

const std::vector<std::string> Factory::GetInputs() const {
    std::vector<std::string> result;

    result = BlueprintManager::Get()->GetBlueprint(blueprintId).GetInput();
    return result;
}

// take one product from equipment and add it to factory equipment
bool Factory::TakeInputsFromInventory(EquipmentComponent& equipment) {
    if (equipment.IsEmpty())
        return false;

    const std::vector<std::string> factoryInputs = GetInputs();

    for (const auto& item : factoryInputs) {
        if (!equipment.Has(item))
            continue;

        if (factoryEquipment->Has(item))
            continue;

        equipment.RequestProduct(item);
        factoryEquipment->AddInput(item);

        return true;
    }

    return false;
}

std::string Factory::GiveOutput() {
    const std::string factoryOutput =
            BlueprintManager::Get()
                    ->GetBlueprint(GetBlueprintId())
                    .GetOutput();

    std::string result;
    if (factoryEquipment->IsOutputPresent()) {
        result = factoryOutput;
        factoryEquipment->RemoveOutput();
    } else {
        result = "None";
    }

    return result;
}

bool Factory::CheckBlueprint() {
    return factoryEquipment->IsAllInputsPresent();
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

    if(result->GetBlueprintId() != "None") {

        auto blueprint = BlueprintManager::Get()->GetBlueprint(result->GetBlueprintId());
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/progress_bar_material2.json");
        result->uiProgress = result->AddComponent<mlg::ProgressBar>("uiProgress", material).lock();
        result->uiProgress->SetBillboardTarget(result);
        result->uiProgress->SetSize({68.f, 68.f});
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

                material = ProductManager::Get()->GetProduct(required[i]).icon;
                temp = result->AddComponent<mlg::Image>("uiRequired", material).lock();
                temp->SetBillboardTarget(result);
                temp->SetSize({14.f, 14.f});
                temp->SetPosition(billboardPos);

                billboardPos.x += 22.f;
            }
        }
    }

    if(result->blueprintId != "None") {
        auto blueprint = BlueprintManager::Get()->GetBlueprint(result->GetBlueprintId());
        material = ProductManager::Get()->GetProduct(blueprint.GetOutput()).icon;
    } else {
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/storage_material.json");
    }
    result->uiIcon = result->AddComponent<mlg::Image>("uiIcon", material).lock();
    result->uiIcon->SetBillboardTarget(result);
    result->uiIcon->SetSize({24.f, 24.f});
    result->uiIcon->SetPosition({0.f, 50.f});
}
