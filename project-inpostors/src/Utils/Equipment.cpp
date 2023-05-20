#include "include/Utils/EquipmentComponent.h"

#include "Utils/Product.h"
#include "Utils/Blueprint.h"

bool EquipmentComponent::AddProduct(const std::shared_ptr<Product>& product) {
    if (equipment.size() >= size)
        return false;

    auto newProduct = std::make_shared<Product>(*product);
    equipment.push_back(newProduct);
    equipmentChanged();
    return true;
}

bool EquipmentComponent::RequestProduct(const std::shared_ptr<Product>& product) {
    for (auto it = equipment.begin(); it != equipment.end(); ++it) {
        if (product->Equals(*it)) {
            equipment.erase(it);
            equipmentChanged();
            return true;
        }
    }
    return false;
}

bool EquipmentComponent::CheckProduct(const std::shared_ptr<Product>& product) const {
    for (const auto& ownedProduct : equipment) {
        if (product->Equals(ownedProduct)) {
            return true;
        }
    }
    return false;
}

bool EquipmentComponent::CheckBlueprint(const Blueprint& blueprint) const {
    for (const auto&  product : blueprint.GetInput()) {
        if (!CheckProduct(product)) {
            return false;
        }
    }
    return true;
}

bool EquipmentComponent::CheckIsFull() const{
    return equipment.size() >= size;
}

EquipmentComponent::EquipmentComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, int size)
: Component(owner, name), size(size) {}

EquipmentComponent::~EquipmentComponent() = default;
