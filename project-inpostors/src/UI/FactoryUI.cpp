#include "UI/FactoryUI.h"

#include "Buildings/Factory.h"
#include "Buildings/FactoryEquipmentComponent.h"
#include "Core/TimerManager.h"
#include "Gameplay/ComponentManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/SceneComponent.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/ProgressBar.h"
#include "Utils/BlueprintManager.h"
#include "Utils/ProductManager.h"
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <spdlog/spdlog.h>

FactoryUI::FactoryUI(const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
    : mlg::SceneComponent(owner, name) {
    factory = std::dynamic_pointer_cast<Factory>(owner.lock());
    MLG_ASSERT_MSG(factory != nullptr, "FactoryUI can only be attached to Factory");

    auto blueprint = BlueprintManager::Get()->GetBlueprint(factory->GetBlueprintId());

    const auto& required = blueprint.GetInput();

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/required_bar_material.json");
    glm::vec2 pos = {-9.f, 25.f};
    for (auto& uiInputBar : uiInputBars) {
        uiInputBar = factory->AddComponent<mlg::Image>("uiInputBar", material).lock();
        uiInputBar->SetSize({18.f, 14.f});
        uiInputBar->SetRelativePosition(pos);
        uiInputBar->SetVisible(false);
        pos.x += 18.f;
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/pin_progress_material.json");
    uiProgress = factory->AddComponent<mlg::ProgressBar>("uiProgress", material).lock();
    uiProgress->SetSize(glm::vec2{60.5f});
    uiProgress->SetPosition({0.f, 40.f});
    uiProgress->percentage = 0.f;

    material = ProductManager::Get()->GetProduct(blueprint.GetOutput()).icon;
    uiIcon = factory->AddComponent<mlg::Image>("uiIcon", material).lock();
    uiIcon->SetSize({24.f, 24.f});
    uiIcon->SetPosition({0.f, 50.f});

    if (!blueprint.GetInput().empty()) {
        auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/required_panel_material.json");
        auto temp = factory->AddComponent<mlg::Image>("uiRequiredPanel", material).lock();
        temp->SetSize({40.f, 18.f});
        temp->SetPosition({0.f, 25.f});
        requiredIcons.push_back(temp);
    }


    glm::vec2 billboardPos = {-11.f * (required.size() - 1.f), 25.f};
    for (const auto& i : required) {
        material = ProductManager::Get()->GetProduct(i).icon;
        auto temp = factory->AddComponent<mlg::Image>("uiRequired", material).lock();
        temp->SetBillboardTarget(owner);
        temp->SetSize({14.f, 14.f});
        temp->SetPosition(billboardPos);

        billboardPos.x += 22.f;

        requiredPanels.push_back(temp);
    }

    if (!blueprint.GetInput().empty()) {
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/arrow_material.json");
        arrow = factory->AddComponent<mlg::Image>("uiArrow", material).lock();
        arrow->SetSize({8.f, 8.f});
        arrow->SetPosition({0.f, 37.f});
    }

}

void FactoryUI::Start() {
    factory->factoryEquipment->inputProductChanged.append([this]() {
       UpdateFactoryInputIcons();
    });

    auto sharedThis =
            mlg::ComponentManager::GetByRawPointer(this).lock();
    auto thisAsSceneComponent =
            std::static_pointer_cast<mlg::SceneComponent>(sharedThis);

    uiProgress->SetBillboardTarget(thisAsSceneComponent);
    uiIcon->SetBillboardTarget(thisAsSceneComponent);

    if (arrow)
        arrow->SetBillboardTarget(thisAsSceneComponent);

    for (auto& uiInputBar : uiInputBars) {
        uiInputBar->SetBillboardTarget(thisAsSceneComponent);
    }
    for (auto& requiredIcon : requiredIcons) {
        requiredIcon->SetBillboardTarget(thisAsSceneComponent);
    }
    for (auto& requiredPanel : requiredPanels) {
        requiredPanel->SetBillboardTarget(thisAsSceneComponent);
    }
}

void FactoryUI::Update() {
    auto bpm = BlueprintManager::Get();
    auto blueprint = bpm->GetBlueprint(factory->blueprintId);

    if (!factory->IsWorking()) {
        uiProgress->percentage = factory->IsOutputPresent();
        AnimatePanel();
        return;
    }

    uiProgress->percentage = mlg::TimerManager::Get()->GetPercentage(
            factory->produceTimerHandle);

}

void FactoryUI::UpdateFactoryInputIcons() {
    auto blueprint = BlueprintManager::Get()->GetBlueprint(this->factory->blueprintId);
    auto factoryEquipment = this->factory->factoryEquipment;
    const auto& inputs = this->factory->GetInputs();

    if (inputs.empty()) {
        return;
    } else if (inputs.size() == 1) {
        for (auto & uiInputBar : uiInputBars) {
            uiInputBar->SetVisible(factoryEquipment->Has(blueprint.GetInput()[0]));
        }
    } else if (inputs.size() == 2) {
        for (int i = 0; i < 2; ++i) {
            uiInputBars[i]->SetVisible(factoryEquipment->Has(blueprint.GetInput()[i]));
        }
    }
}

void FactoryUI::AnimatePanel() {
    if (!factory->IsOutputPresent())
        return;

    glm::vec3 position = GetTransform().GetPosition();

    const float speed = 8.f;
    const float amplitude = 1.5f;

    position.y = (float) glm::sin(mlg::Time::GetSeconds() * speed) * amplitude;
    GetTransform().SetPosition(position);
}
