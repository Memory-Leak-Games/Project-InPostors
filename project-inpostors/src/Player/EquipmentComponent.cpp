#include <string>

#include "Utils/Blueprint.h"
#include "Utils/BlueprintManager.h"
#include "Player/EquipmentComponent.h"

bool EquipmentComponent::AddProduct(const std::string& product) {
    if (equipment.size() >= size)
        return false;

    equipment.push_back(product);
    equipmentChanged();
    productAdded();
    return true;
}

bool EquipmentComponent::RequestProduct(const std::string& product) {
    const auto& foundIterator = std::find(equipment.begin(), equipment.end(), product);
    if (foundIterator == equipment.end())
        return false;

    equipment.erase(foundIterator);
    equipmentChanged();
    productRemoved();
    return true;
}

std::string EquipmentComponent::RequestOldestProduct() {
    if (equipment.empty())
        return "none";

    std::string product = equipment.front();
    equipment.erase(equipment.begin());
    equipmentChanged();
    productRemoved();

    return product;
}

std::string EquipmentComponent::RequestProduct() {
    if (equipment.empty())
        return "none";

    std::string product = equipment.back();
    equipment.pop_back();
    equipmentChanged();
    productRemoved();
    
    return product;
}

const std::vector<std::string>& EquipmentComponent::GetProducts() const {
    return equipment;
}

bool EquipmentComponent::Has(const std::string& product) const {
    return std::find(equipment.begin(), equipment.end(), product) != equipment.end();
}

int EquipmentComponent::GetNumberOfProduct(const std::string& productId) const {
    int count = 0;
    for (const auto& product : equipment) {
        if (product == productId)
            count++;
    }

    return count;
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

bool EquipmentComponent::IsEmpty() const {
    return equipment.empty();
}

const std::vector<std::string> EquipmentComponent::GetEquipment() const {
    return equipment;
}

EquipmentComponent::EquipmentComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, int size)
    : Component(owner, name), size(size) {}

EquipmentComponent::~EquipmentComponent() = default;
