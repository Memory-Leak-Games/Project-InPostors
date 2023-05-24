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

#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/ProgressBar.h"
#include "UI/Components/Label.h"
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

    GenerateUI(result);

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

    if(result->factoryType == FactoryType::OneInput || result->factoryType == FactoryType::OneOutput) {
        auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/factory/panel_material.json");
        auto uiPanel = result->AddComponent<mlg::Image>("PanelRecipe", material).lock();
        uiPanel->SetSize({48.f, 36.f});
        uiPanel->SetPosition({0.f, 75.f-16.f});
        uiPanel->SetBillboardTarget(result);

//        uiPanel = result->AddComponent<mlg::Image>("PanelRes", material).lock();
//        uiPanel->SetSize({64.f, 20.f});
//        uiPanel->SetPosition({0.f, 40.f});
//        uiPanel->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/factory/progressBar_material.json");
        auto uiBar = result->AddComponent<mlg::ProgressBar>("Bar", material).lock();
        uiBar->SetSize({48.f, 5.f});
        uiBar->SetPosition({0.f, 59.f-16.f});
        uiBar->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/icon/wood_material.json");
        auto uiWood = result->AddComponent<mlg::Image>("Wood", material).lock();
        uiWood->SetSize({24.f, 24.f});
        uiWood->SetPosition({0.f, 75.f-16.f});
        uiWood->SetBillboardTarget(result);

        auto uiEq = result->AddComponent<mlg::Image>("Eq", material).lock();
        uiEq->SetSize({16.f, 16.f});
        uiEq->SetPosition({0.f-12.f, 44.f-16.f});
        uiEq->SetBillboardTarget(result);

        result->amount1 = result->AddComponent<mlg::Label>("Amount1", font).lock();
        result->amount1->SetSize(18);
        result->amount1->SetPosition({0.f, 40.f-16.f});
        result->amount1->SetText("x1");
        result->amount1->SetTextColor({1.0, 0.8, 0.0});
        result->amount1->SetBillboardTarget(result);
    } else {
        auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/factory/panel_material.json");
        auto uiPanel = result->AddComponent<mlg::Image>("PanelRecipe", material).lock();
        uiPanel->SetSize({128.f, 36.f});
        uiPanel->SetPosition({0.f, 75.f-16.f});
        uiPanel->SetBillboardTarget(result);

//        uiPanel = result->AddComponent<mlg::Image>("PanelRes", material).lock();
//        uiPanel->SetSize({64.f, 20.f});
//        uiPanel->SetPosition({-36.f, 40.f});
//        uiPanel->SetBillboardTarget(result);
//
//        uiPanel = result->AddComponent<mlg::Image>("PanelProd", material).lock();
//        uiPanel->SetSize({64.f, 20.f});
//        uiPanel->SetPosition({36.f, 40.f});
//        uiPanel->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/factory/progressBar_material.json");
        auto uiBar = result->AddComponent<mlg::ProgressBar>("Bar", material).lock();
        uiBar->SetSize({128.f, 5.f});
        uiBar->SetPosition({0.f, 59.f-16.f});
        uiBar->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/icon/wood_material.json");
        auto uiWood = result->AddComponent<mlg::Image>("Recipe1", material).lock();
        uiWood->SetSize({24.f, 24.f});
        uiWood->SetPosition({-48.f, 75.f-16.f});
        uiWood->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/icon/metal_material.json");
        auto uiMetal = result->AddComponent<mlg::Image>("Recipe2", material).lock();
        uiMetal->SetSize({24.f, 24.f});
        uiMetal->SetPosition({-16.f, 75.f-16.f});
        uiMetal->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/icon/arrow_material.json");
        auto uiArrow = result->AddComponent<mlg::Image>("RecipeArrow", material).lock();
        uiArrow->SetSize({24.f, 24.f});
        uiArrow->SetPosition({16.f, 75.f-16.f});
        uiArrow->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/icon/furniture_material.json");
        auto uiFurniture = result->AddComponent<mlg::Image>("Recipe3", material).lock();
        uiFurniture->SetSize({24.f, 24.f});
        uiFurniture->SetPosition({48.f, 75.f-16.f});
        uiFurniture->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/icon/wood_material.json");
        auto uiEq = result->AddComponent<mlg::Image>("Eq", material).lock();
        uiEq->SetSize({16.f, 16.f});
        uiEq->SetPosition({-42.f-12.f, 44.f-16.f});
        uiEq->SetBillboardTarget(result);

        result->amount1 = result->AddComponent<mlg::Label>("Amount1", font).lock();
        result->amount1->SetSize(18);
        result->amount1->SetPosition({-42.f, 40.f-16.f});
        result->amount1->SetText("x1");
        result->amount1->SetTextColor({1.0, 0.8, 0.0});
        result->amount1->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/icon/metal_material.json");
        uiEq = result->AddComponent<mlg::Image>("Eq", material).lock();
        uiEq->SetSize({16.f, 16.f});
        uiEq->SetPosition({0.f-12.f, 44.f-16.f});
        uiEq->SetBillboardTarget(result);

        result->amount2 = result->AddComponent<mlg::Label>("Amount2", font).lock();
        result->amount2->SetSize(18);
        result->amount2->SetPosition({0.f, 40.f-16.f});
        result->amount2->SetText("x1");
        result->amount2->SetTextColor({0.8, 0.8, 1.0});
        result->amount2->SetBillboardTarget(result);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/icon/furniture_material.json");
        uiEq = result->AddComponent<mlg::Image>("Eq", material).lock();
        uiEq->SetSize({16.f, 16.f});
        uiEq->SetPosition({42.f-12.f, 44.f-16.f});
        uiEq->SetBillboardTarget(result);

        result->amount3 = result->AddComponent<mlg::Label>("Amount3", font).lock();
        result->amount3->SetSize(18);
        result->amount3->SetPosition({42.f, 40.f-16.f});
        result->amount3->SetText("x1");
        result->amount3->SetTextColor({1.0, 0.5, 0.1});
        result->amount3->SetBillboardTarget(result);
    }
}
