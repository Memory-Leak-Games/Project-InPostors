#include "Buildings/FactoryEquipmentComponent.h"

#include "Utils/Blueprint.h"
#include "Utils/BlueprintManager.h"

FactoryEquipmentComponent::FactoryEquipmentComponent(
        const std::weak_ptr<mlg::Entity>& owner,
        const std::string& name,
        const std::string& blueprintId)
    : Component(owner, name) {
    const Blueprint& blueprint = BlueprintManager::Get()
                                         ->GetBlueprint(blueprintId);

    for (const auto& input : blueprint.GetInput()) {
        inputs[input] = false;
    }

    output = std::make_pair(blueprint.GetOutput(), false);
}

FactoryEquipmentComponent::~FactoryEquipmentComponent() = default;

void FactoryEquipmentComponent::Produce() {
    output.second = true;
}

void FactoryEquipmentComponent::AddInput(const std::string& productId) {
    inputs.at(productId) = true;
    inputProductAdded();
    inputProductChanged();
}

void FactoryEquipmentComponent::ClearInput() {
    for (auto& input : inputs) {
        input.second = false;
    }
    inputProductChanged();
}

void FactoryEquipmentComponent::RemoveOutput() {
    output.second = false;
    outputProductRemoved();
}

bool FactoryEquipmentComponent::IsAllInputsPresent() const {
    return std::ranges::all_of(inputs, [](const auto& input) {
        return input.second;
    });
}

bool FactoryEquipmentComponent::IsOutputPresent() const {
    return output.second;
}

bool FactoryEquipmentComponent::CanStoreInput(const std::string& productId) const {
    return !inputs.at(productId);
}

bool FactoryEquipmentComponent::Has(const std::string& productId) const {
    return inputs.at(productId);
}

