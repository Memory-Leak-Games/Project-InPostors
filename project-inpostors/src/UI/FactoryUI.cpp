#include "UI/FactoryUI.h"

#include "Buildings/Factory.h"
#include "Buildings/FactoryEquipmentComponent.h"
#include "Core/TimerManager.h"
#include "Gameplay/Entity.h"
#include "Player.h"
#include "UI/Components/Label.h"
#include "UI/Components/ProgressBar.h"
#include "Utils/BlueprintManager.h"
#include "Utils/EquipmentComponent.h"
#include "Utils/ProductManager.h"
#include "include/Rendering/Assets/MaterialAsset.h"
#include "include/UI/Components/Image.h"

FactoryUI::FactoryUI(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, const std::shared_ptr<Factory>& factory)
    : Component(owner, name), factory(factory) {

    auto blueprint = BlueprintManager::Get()->GetBlueprint(factory->GetBlueprintId());
    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/pin_material.json");

    uiPin = factory->AddComponent<mlg::Image>("uiPin", material).lock();
    uiPin->SetBillboardTarget(owner);
    uiPin->SetSize({88.f, 88.f});
    uiPin->SetPosition({0.f, 50.f});

    const auto& required = blueprint.GetInput();

    if (!blueprint.GetInput().empty()) {
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/required_panel_material.json");
        auto temp = factory->AddComponent<mlg::Image>("uiRequiredPanel", material).lock();
        temp->SetBillboardTarget(owner);
        temp->SetSize({40.f, 18.f});
        temp->SetPosition({0.f, 25.f});
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/required_bar_material.json");
    glm::vec2 pos = {-9.f, 25.f};
    for (auto & uiInputBar : uiInputBars) {
        uiInputBar = factory->AddComponent<mlg::Image>("uiInputBar", material).lock();
        uiInputBar->SetSize({18.f, 14.f});
        uiInputBar->SetRelativePosition(pos);
        uiInputBar->SetBillboardTarget(owner);
        uiInputBar->SetVisible(false);
        pos.x += 18.f;
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/pin_progress_material.json");
    uiProgress = factory->AddComponent<mlg::ProgressBar>("uiProgress", material).lock();
    uiProgress->SetBillboardTarget(owner);
    uiProgress->SetSize({74.f, 74.f});
    uiProgress->SetPosition({0.f, 50.f});
    uiProgress->percentage = 0.f;

    material = ProductManager::Get()->GetProduct(blueprint.GetOutput()).icon;
    uiIcon = factory->AddComponent<mlg::Image>("uiIcon", material).lock();
    uiIcon->SetBillboardTarget(owner);
    uiIcon->SetSize({24.f, 24.f});
    uiIcon->SetPosition({0.f, 50.f});

    glm::vec2 billboardPos = {-11.f * (required.size() - 1.f), 25.f};
    for(const auto & i : required) {

        material = ProductManager::Get()->GetProduct(i).icon;
        auto temp = factory->AddComponent<mlg::Image>("uiRequired", material).lock();
        temp->SetBillboardTarget(owner);
        temp->SetSize({14.f, 14.f});
        temp->SetPosition(billboardPos);

        billboardPos.x += 22.f;
    }

    if (!blueprint.GetInput().empty()) {
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/arrow_material.json");
        auto temp = factory->AddComponent<mlg::Image>("uiArrow", material).lock();
        temp->SetBillboardTarget(owner);
        temp->SetSize({8.f, 8.f});
        temp->SetPosition({0.f, 37.f});
    }
}

void FactoryUI::Update() {
    auto bpm = BlueprintManager::Get();
    auto blueprint = bpm->GetBlueprint(factory->blueprintId);

    if (factory->IsWorking()) {
        float produceElapsed = mlg::TimerManager::Get()->GetTimerElapsedTime(factory->produceTimerHandle);
        float timeToProcess = bpm->GetBlueprint(factory->blueprintId).GetTimeToProcess();
        float timeRate = produceElapsed / timeToProcess;
        uiProgress->percentage = timeRate * 0.75f;
    } else {
        uiProgress->percentage = factory->factoryEquipment->IsOutputPresent();
    }
}

void FactoryUI::UpdateFactoryInputIcons() {
    auto blueprint = BlueprintManager::Get()->GetBlueprint(this->factory->blueprintId);
    auto factoryEquipment = this->factory->factoryEquipment;
    const auto& inputs = this->factory->GetInputs();
    if(inputs.empty()) {
        return;
    } else if(inputs.size() == 1) {
        for (int i = 0; i < 2; ++i) {
            uiInputBars[i]->SetVisible(factoryEquipment->Has(blueprint.GetInput()[0]));
        }
    } else if (inputs.size() == 2) {
        for (int i = 0; i < 2; ++i) {
            uiInputBars[i]->SetVisible(factoryEquipment->Has(blueprint.GetInput()[i]));
        }
    }
}
