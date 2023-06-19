#include "include/UI/FactoryUI.h"

#include "Gameplay/Entity.h"
#include "Player.h"
#include "include/UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/ProgressBar.h"
#include "include/Rendering/Assets/MaterialAsset.h"
#include "Utils/EquipmentComponent.h"
#include "Utils/BlueprintManager.h"
#include "Utils/ProductManager.h"
#include "Buildings/Factory.h"
#include "Core/TimerManager.h"
#include "Buildings/FactoryEquipmentComponent.h"

FactoryUI::FactoryUI(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, const std::shared_ptr<Factory>& factory)
    : Component(owner, name), factory(factory) {

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/pin_material.json");

    uiPin = factory->AddComponent<mlg::Image>("uiPin", material).lock();
    uiPin->SetBillboardTarget(owner);
    uiPin->SetSize({82.f, 82.f});
    uiPin->SetPosition({0.f, 50.f});

    if(factory->GetBlueprintId() != "None") {

        auto blueprint = BlueprintManager::Get()->GetBlueprint(factory->GetBlueprintId());
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/progress_bar_material2.json");
        uiProgress = factory->AddComponent<mlg::ProgressBar>("uiProgress", material).lock();
        uiProgress->SetBillboardTarget(owner);
        uiProgress->SetSize({68.f, 68.f});
        uiProgress->SetPosition({0.f, 50.f});
        uiProgress->percentage = 0.f;

        if (!blueprint.GetInput().empty()) {
            const auto& required = blueprint.GetInput();

            material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/required_panel_material.json");
            auto temp = factory->AddComponent<mlg::Image>("uiRequiredPanel", material).lock();
            temp->SetBillboardTarget(owner);
            temp->SetSize({42.f, 16.f});
            temp->SetPosition({0.f, 30.f});

            glm::vec2 billboardPos = {-11.f * (required.size() - 1.f), 30.f};
            for(const auto & i : required) {

                material = ProductManager::Get()->GetProduct(i).icon;
                temp = factory->AddComponent<mlg::Image>("uiRequired", material).lock();
                temp->SetBillboardTarget(owner);
                temp->SetSize({14.f, 14.f});
                temp->SetPosition(billboardPos);

                billboardPos.x += 22.f;
            }
        }
    }

    if(factory->blueprintId != "None") {
        auto blueprint = BlueprintManager::Get()->GetBlueprint(factory->GetBlueprintId());
        material = ProductManager::Get()->GetProduct(blueprint.GetOutput()).icon;
    } else {
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/storage_material.json");
    }
    uiIcon = factory->AddComponent<mlg::Image>("uiIcon", material).lock();
    uiIcon->SetBillboardTarget(owner);
    uiIcon->SetSize({24.f, 24.f});
    uiIcon->SetPosition({0.f, 50.f});
}

void FactoryUI::Update() {
    if (!factory->blueprintId.empty()) {
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
}
