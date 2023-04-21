#include "Buildings/FactorySplit.h"

#include "Utils/Blueprint.h"
#include "Car/Equipment.h"

void FactorySplit::UnloadProduct(Equipment& equipment) {
    for (const auto& input : blueprint->GetInput()) {
        if (equipment.RequestProduct(input)) {
            factoryEquipment->AddProduct(input);
            return;
        }
    }
}

void FactorySplit::LoadProduct(Equipment& equipment) {
    if (equipment.CheckIsFull()) {
        return;
    }

    if (!factoryEquipment->CheckBlueprint(*blueprint)) {
        return;
    }

    for (const auto& product : blueprint->GetInput()) {
        factoryEquipment->RequestProduct(product);
    }

    equipment.AddProduct(blueprint->GetOutput());
}

FactorySplit::~FactorySplit() = default;

FactorySplit::FactorySplit() = default;
