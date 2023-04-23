#pragma once

#include "Building.h"

class FactorySplit : public Building {
private:
    std::unique_ptr<class Blueprint> blueprint;
    std::unique_ptr<class Equipment> factoryEquipment;
public:
    FactorySplit();
    ~FactorySplit();

    void UnloadProduct(Equipment& equipment) override;
    void LoadProduct(Equipment& equipment) override;
};

