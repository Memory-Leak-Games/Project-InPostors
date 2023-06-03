#include <string>

#include "Utils/Blueprint.h"
#include "Utils/BlueprintManager.h"
#include "Utils/EquipmentComponent.h"

bool EquipmentComponent::AddProduct(const std::string& product) {
    if (equipment.size() >= size)
        return false;

    equipment.push_back(product);
    equipmentChanged();
    return true;
}

bool EquipmentComponent::RequestProduct(const std::string& product) {
    const auto& foundIterator = std::find(equipment.begin(), equipment.end(), product);
    if (foundIterator == equipment.end())
        return false;

    equipment.erase(foundIterator);
    equipmentChanged();
    return true;
}

std::string EquipmentComponent::RequestProduct() {
    if (equipment.empty())
        return "none";

    std::string product = equipment.front();
    equipment.pop_back();
    equipmentChanged();
    
    return product;
}

bool EquipmentComponent::Has(const std::string& product) const {
    return std::find(equipment.begin(), equipment.end(), product) != equipment.end();
}

std::string EquipmentComponent::ToString() const {
    std::string result = "Eq: ";

    for (const auto& item : equipment) {
        result += item + ", ";
    }

    return result;
}

bool EquipmentComponent::IsFull() const {
    return equipment.size() >= size;
}

const std::vector<std::string> EquipmentComponent::GetEquipment() const {
    return equipment;
}

EquipmentComponent::EquipmentComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, int size)
    : Component(owner, name), size(size) {}

EquipmentComponent::~EquipmentComponent() = default;
