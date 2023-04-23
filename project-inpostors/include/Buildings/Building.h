#pragma once

class Building {
public:
    virtual void LoadProduct(class Equipment& equipment) = 0;
    virtual void UnloadProduct(Equipment& equipment) = 0;
    virtual ~Building() = default;
};
