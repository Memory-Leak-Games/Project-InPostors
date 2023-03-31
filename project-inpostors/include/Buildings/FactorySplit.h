#pragma once

#include "Building.h"

class FactorySplit : public Building {
private:
    Blueprint blueprint;
    Equipment* factoryEquipment;

    void UnloadProduct(Equipment& equipment) override = 0;
    void LoadProduct(Equipment& equipment) override = 0;
};

