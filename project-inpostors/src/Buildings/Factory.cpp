#include "Buildings/Factory.h"

#include <fstream>
#include <spdlog/spdlog.h>
#include <vector>

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
    equipmentComponent->equipmentChanged.remove(equipmentChangedHandle);

    mlg::Scene* currentScene = mlg::SceneManager::GetCurrentScene();
    auto* levelScene = dynamic_cast<LevelScene*>(currentScene);

    std::vector<std::string> allProducts = equipmentComponent->GetProducts();
    std::string productId = levelScene->GetTaskManager()->FinishTask(allProducts);
    equipmentComponent->RequestProduct(productId);

    // sell rest of products
    allProducts = equipmentComponent->GetProducts();
    for (const auto& product : allProducts) {
        levelScene->GetTaskManager()->SellProduct(product);
        equipmentComponent->RequestProduct(product);
    }

    SetStorageEquipmentCallback();
}

void Factory::Start() {
    if (factoryType == FactoryType::Storage)
        SetStorageEquipmentCallback();
    else
        StartAsFactory();
}

void Factory::SetStorageEquipmentCallback() {
    equipmentChangedHandle =
        equipmentComponent->equipmentChanged.append([this]() {
            FinishTask();
        });
}

void Factory::StartAsFactory() {
    CheckBlueprintAndStartWorking();

    createProductSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/create_product.wav");

    equipmentChangedHandle =
        equipmentComponent->equipmentChanged.append([this]() {
            CheckBlueprintAndStartWorking();
        });
}

void Factory::Update() {
    UpdateUi();

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

void Factory::UpdateUi() {
    if (blueprintId != "None") {
        auto blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);
        float produceElapsed = mlg::TimerManager::Get()->GetTimerElapsedTime(produceTimerHandle);
        float timeToProcess = BlueprintManager::Get()->GetBlueprint(blueprintId).GetTimeToProcess();
        float timeRate = produceElapsed / timeToProcess;

        // please refactor this wide lines 🙏
        if (!blueprint.GetInput().empty() && barReq1) {
            barReq1->percentage = equipmentComponent->GetNumberOfProduct(blueprint.GetInput()[0]) / 3.0f + (timeRate > 0.0f) * 0.33f;
        }

        if (blueprint.GetInput().size() > 1 && barReq2) {
            barReq2->percentage = equipmentComponent->GetNumberOfProduct(blueprint.GetInput()[1]) / 3.0f + (timeRate > 0.0f) * 0.33f;
        }

        if (barArrow) {
            barArrow->percentage = timeRate;
        }

        if (barRes) {
            barRes->percentage = equipmentComponent->GetNumberOfProduct(blueprint.GetOutput()) / 3.0;
        }
    }
}

bool Factory::IsWorking() const {
    return mlg::TimerManager::Get()->IsTimerValid(produceTimerHandle);
}

// if factory is storage, return all tasks products, else return blueprint input
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

// take one product from equipment and add it to factory equipment
bool Factory::TakeInputsFromInventory(EquipmentComponent& equipment) {
    if (equipment.IsEmpty())
        return false;

    if (factoryType == FactoryType::Storage) {
        return TakeInputsAsStorage(equipment);
    } else {
        return TakeInputsAsFactory(equipment);
    }
}

bool Factory::TakeInputsAsStorage(EquipmentComponent& equipment) {
    const std::vector<std::string> factoryInputs = GetInputs();

    if (TakeInputsAsFactory(equipment))
        return true;

    std::string productId = equipment.RequestProduct();
    this->equipmentComponent->AddProduct(productId);

    return true;
}

bool Factory::TakeInputsAsFactory(EquipmentComponent& equipment) {
    const std::vector<std::string> factoryInputs = GetInputs();

    for (const auto& item : factoryInputs) {
        if (!equipment.Has(item))
            continue;

        if (!GetEquipmentComponent()->AddProduct(item))
            continue;

        equipment.RequestProduct(item);

        return true;
    }

    return false;
}

const std::shared_ptr<EquipmentComponent>& Factory::GetEquipmentComponent() const {
    return equipmentComponent;
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
                result->barArrow->SetPosition({16.f, 75.f - 16.f});
                result->barArrow->SetBillboardTarget(result);

                material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/panel_material.json");
                result->barReq1 = result->AddComponent<mlg::ProgressBar>("BarReq1", material).lock();
                result->barReq1->SetSize({32.f, 32.f});
                result->barReq1->SetPosition({-48.f, 75.f - 16.f});
                result->barReq1->SetBillboardTarget(result);

                material = ProductManager::Get()->GetProduct(blueprint.GetInput()[0]).icon;
                auto iconReq1 = result->AddComponent<mlg::Image>("IconReq1", material).lock();
                iconReq1->SetSize({24.f, 24.f});
                iconReq1->SetPosition({-48.f, 75.f - 16.f});
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
                    result->barRes->SetPosition({48.f, 75.f - 16.f});
                    iconRes->SetPosition({48.f, 75.f - 16.f});
                    result->barArrow->SetPosition({16.f, 75.f - 16.f});
                    result->barReq1->SetPosition({-48.f, 75.f - 16.f});
                    iconReq1->SetPosition({-48.f, 75.f - 16.f});
                    result->barReq2->SetPosition({-16.f, 75.f - 16.f});
                    iconReq2->SetPosition({-16.f, 75.f - 16.f});
                } else {
                    // Positioning for 1 input and 1 output
                    result->barRes->SetPosition({32.f, 75.f - 16.f});
                    iconRes->SetPosition({32.f, 75.f - 16.f});
                    result->barArrow->SetPosition({0.f, 75.f - 16.f});
                    result->barReq1->SetPosition({-32.f, 75.f - 16.f});
                    iconReq1->SetPosition({-32.f, 75.f - 16.f});
                }
            } else {
                // Positioning for only output
                result->barRes->SetPosition({0.f, 75.f - 16.f});
                iconRes->SetPosition({0.f, 75.f - 16.f});
            }
        }

    } else if (result->factoryType == FactoryType::Storage) {
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/task_panel_material.json");
        auto temp = result->AddComponent<mlg::Image>("Storage", material).lock();
        temp->SetSize({64.f, 64.f});
        temp->SetBillboardTarget(result);
        temp->SetPosition({0.f, 112.f});

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/storage_material.json");
        temp = result->AddComponent<mlg::Image>("StorageIcon", material).lock();
        temp->SetSize({20.f, 20.f});
        temp->SetBillboardTarget(result);
        temp->SetPosition({0.f, 112.f});
    }
}
