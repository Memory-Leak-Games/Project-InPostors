#pragma once

#include "include/Car/Equipment.h"

class Building {
public:
    virtual void LoadProduct(Equipment& equipment) = 0;
    virtual void UnloadProduct(Equipment& equipment) = 0;
    virtual ~Building() = default;
};
