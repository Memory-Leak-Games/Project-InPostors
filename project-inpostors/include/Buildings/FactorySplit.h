#pragma once

#include "Building.h"

class FactorySplit : public Building {
private:
    Blueprint blueprint;
    Equipment factoryEquipment;
public:
    void UnloadProduct(Equipment& equipment) override;
    void LoadProduct(Equipment& equipment) override;
};

