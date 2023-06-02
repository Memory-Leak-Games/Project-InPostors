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

#include "Core/Time.h"
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

    GenerateUI(result);

    return result;
}

void Factory::AddMesh(const json& staticMeshJson) {
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(staticMeshJson["model"].get<std::string>());
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(staticMeshJson["material"].get<std::string>());

    auto staticMeshComponent = AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);
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
            emitterJson["position"][2]
    };
    emitterPosition.x += meshOffset.x;
    emitterPosition.z += meshOffset.y;
    emitterComponent.lock()->GetTransform().SetPosition(emitterPosition);
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
    if (mlg::TimerManager::Get()->IsTimerValid(produceTimerHandle) || equipmentComponent->IsFull())
        return;

    const Blueprint& blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);

    if (!blueprint.CheckBlueprint(*equipmentComponent))
        return;
    
    // Remove inputs from eq
    for (const auto& item : blueprint.GetInput()) {
        equipmentComponent->RequestProduct(item);
    }

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

    float produceElapsed = mlg::TimerManager::Get()->GetTimerElapsedTime(produceTimerHandle);
    float timeToProcess = BlueprintManager::Get()->GetBlueprint(blueprintId).GetTimeToProcess();

    auto blueprint = BlueprintManager::Get()->GetBlueprint(blueprintId);
    if (!blueprint.GetInput().empty() && barReq1)
        barReq1->percentage = equipmentComponent->Has(blueprint.GetInput()[0]) /*|| produceElapsed >= 0.0*/;
    if (blueprint.GetInput().size() > 1 && barReq2)
        barReq2->percentage = equipmentComponent->Has(blueprint.GetInput()[1]) /*|| produceElapsed >= 0.0*/;
    if (barArrow)
        barArrow->percentage = produceElapsed / timeToProcess;
    if (barRes)
        barRes->percentage = equipmentComponent->Has(blueprint.GetOutput());
}

bool Factory::IsWorking() const {
    return mlg::TimerManager::Get()->IsTimerValid(produceTimerHandle);
}

const std::shared_ptr<EquipmentComponent>& Factory::GetEquipmentComponent() const {
    return equipmentComponent;
}

std::string Factory::GetBlueprintId() const { return blueprintId; }

void Factory::GenerateUI(const std::shared_ptr<Factory>& result) {

    auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/arialbd.ttf");
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/panel_material.json");
    auto blueprint = BlueprintManager::Get()->GetBlueprint(result->GetBlueprintId());

    switch (result->factoryType) {

        case FactoryType::OneOutput: {
            result->barRes = result->AddComponent<mlg::ProgressBar>("PanelRecipe", material).lock();
            result->barRes->SetSize({32.f, 32.f});
            result->barRes->SetPosition({0.f, 75.f - 16.f});
            result->barRes->SetBillboardTarget(result);

            material = ProductManager::GetInstance()->GetProduct(blueprint.GetOutput()).icon;
            auto uiWood = result->AddComponent<mlg::Image>("Wood", material).lock();
            uiWood->SetSize({24.f, 24.f});
            uiWood->SetPosition({0.f, 75.f - 16.f});
            uiWood->SetBillboardTarget(result);
            break;
        }

        case FactoryType::OneInputOutput: {
            result->barReq1 = result->AddComponent<mlg::ProgressBar>("BarReq1", material).lock();
            result->barReq1->SetSize({32.f, 32.f});
            result->barReq1->SetPosition({-48.f, 75.f - 16.f});
            result->barReq1->SetBillboardTarget(result);

            result->barReq2 = result->AddComponent<mlg::ProgressBar>("BarReq2", material).lock();
            result->barReq2->SetSize({32.f, 32.f});
            result->barReq2->SetPosition({-16.f, 75.f - 16.f});
            result->barReq2->SetBillboardTarget(result);

            result->barRes = result->AddComponent<mlg::ProgressBar>("BarRes", material).lock();
            result->barRes->SetSize({32.f, 32.f});
            result->barRes->SetPosition({48.f, 75.f - 16.f});
            result->barRes->SetBillboardTarget(result);

            material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/arrow_panel_material.json");
            result->barArrow = result->AddComponent<mlg::ProgressBar>("RecipeArrow", material).lock();
            result->barArrow->SetSize({32.f, 32.f});
            result->barArrow->SetPosition({16.f, 75.f - 16.f});
            result->barArrow->SetBillboardTarget(result);

            material = ProductManager::GetInstance()->GetProduct(blueprint.GetInput()[0]).icon;
            auto icon = result->AddComponent<mlg::Image>("Recipe1", material).lock();
            icon->SetSize({24.f, 24.f});
            icon->SetPosition({-48.f, 75.f - 16.f});
            icon->SetBillboardTarget(result);

            if(blueprint.GetInput().size() >= 2)
            {
                material = ProductManager::GetInstance()->GetProduct(blueprint.GetInput()[1]).icon;
                icon = result->AddComponent<mlg::Image>("Recipe2", material).lock();
                icon->SetSize({24.f, 24.f});
                icon->SetPosition({-16.f, 75.f - 16.f});
                icon->SetBillboardTarget(result);
            }

            material = ProductManager::GetInstance()->GetProduct(blueprint.GetOutput()).icon;
            icon = result->AddComponent<mlg::Image>("Recipe3", material).lock();
            icon->SetSize({24.f, 24.f});
            icon->SetPosition({48.f, 75.f - 16.f});
            icon->SetBillboardTarget(result);
            break;
        }
    }
}
