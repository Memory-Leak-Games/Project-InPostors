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
        float produceElapsed = mlg::TimerManager::Get()->GetTimerElapsedTime(produceTimerHandle);
        float timeToProcess = BlueprintManager::Get()->GetBlueprint(blueprintId).GetTimeToProcess();
        float timeRate = produceElapsed / timeToProcess;

        if (!blueprint.GetInput().empty() && barReq1) {
            barReq1->percentage = factoryEquipment->Has(blueprint.GetInput()[0]);
        }

        if (blueprint.GetInput().size() > 1 && barReq2) {
            barReq1->percentage = factoryEquipment->Has(blueprint.GetInput()[1]);
        }

        if (barArrow) {
            barArrow->percentage = timeRate;
        }

        if (barRes) {
            barRes->percentage = factoryEquipment->IsOutputPresent();
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
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/panel_material.json");

    if (result->GetBlueprintId() != "None") {

        auto blueprint = BlueprintManager::Get()->GetBlueprint(result->GetBlueprintId());

        // Lack of containers gave birth to this monstrosity
        // ave maria purissima ora pro nobis peccatoribus nunc et in hora mortis nostrae amen
        // ~ Github copilot
        if (blueprint.GetOutput() != "") {
            result->barRes = result->AddComponent<mlg::ProgressBar>("BarRes", material).lock();
            result->barRes->SetSize({32.f, 32.f});
            result->barRes->SetBillboardTarget(result);

            material = ProductManager::Get()->GetProduct(blueprint.GetOutput()).icon;
            auto iconRes = result->AddComponent<mlg::Image>("IconRes", material).lock();
            iconRes->SetSize({24.f, 24.f});
            iconRes->SetBillboardTarget(result);

            if (blueprint.GetInput().size() > 0) {
                material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/arrow_panel_material.json");
                result->barArrow = result->AddComponent<mlg::ProgressBar>("RecipeArrow", material).lock();
                result->barArrow->SetSize({32.f, 32.f});
                result->barArrow->SetRelativePosition({16.f, 75.f - 16.f});
                result->barArrow->SetBillboardTarget(result);

                material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/panel_material.json");
                result->barReq1 = result->AddComponent<mlg::ProgressBar>("BarReq1", material).lock();
                result->barReq1->SetSize({32.f, 32.f});
                result->barReq1->SetRelativePosition({-48.f, 75.f - 16.f});
                result->barReq1->SetBillboardTarget(result);

                material = ProductManager::Get()->GetProduct(blueprint.GetInput()[0]).icon;
                auto iconReq1 = result->AddComponent<mlg::Image>("IconReq1", material).lock();
                iconReq1->SetSize({24.f, 24.f});
                iconReq1->SetRelativePosition({-48.f, 75.f - 16.f});
                iconReq1->SetBillboardTarget(result);

                if (blueprint.GetInput().size() > 1) {
                    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/panel_material.json");
                    result->barReq2 = result->AddComponent<mlg::ProgressBar>("BarReq2", material).lock();
                    result->barReq2->SetSize({32.f, 32.f});
                    result->barReq2->SetBillboardTarget(result);

                    material = ProductManager::Get()->GetProduct(blueprint.GetInput()[1]).icon;
                    auto iconReq2 = result->AddComponent<mlg::Image>("IconReq2", material).lock();
                    iconReq2->SetSize({24.f, 24.f});
                    iconReq2->SetBillboardTarget(result);

                    // Positioning for 2 inputs and 1 output
                    result->barRes->SetRelativePosition({48.f, 75.f - 16.f});
                    iconRes->SetRelativePosition({48.f, 75.f - 16.f});
                    result->barArrow->SetRelativePosition({16.f, 75.f - 16.f});
                    result->barReq1->SetRelativePosition({-48.f, 75.f - 16.f});
                    iconReq1->SetRelativePosition({-48.f, 75.f - 16.f});
                    result->barReq2->SetRelativePosition({-16.f, 75.f - 16.f});
                    iconReq2->SetRelativePosition({-16.f, 75.f - 16.f});
                } else {
                    // Positioning for 1 input and 1 output
                    result->barRes->SetRelativePosition({32.f, 75.f - 16.f});
                    iconRes->SetRelativePosition({32.f, 75.f - 16.f});
                    result->barArrow->SetRelativePosition({0.f, 75.f - 16.f});
                    result->barReq1->SetRelativePosition({-32.f, 75.f - 16.f});
                    iconReq1->SetRelativePosition({-32.f, 75.f - 16.f});
                }
            } else {
                // Positioning for only output
                result->barRes->SetRelativePosition({0.f, 75.f - 16.f});
                iconRes->SetRelativePosition({0.f, 75.f - 16.f});
            }
        }
    }
}
