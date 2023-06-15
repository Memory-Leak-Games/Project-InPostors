#pragma once

#include "Buildings/InteractiveBuilding.h"
#include <string>

class Storage : public InteractiveBuilding {
private:
    Storage(
            uint64_t id,
            const std::string& name,
            bool isStatic,
            mlg::Transform* parent);

public:
    ~Storage() override;

    static std::shared_ptr<Storage> Create(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent,
            const std::string& configPath);

    bool TakeInputsFromInventory(class EquipmentComponent& equipment) override;

private:
    void GenerateUI();

    void FinishTaskOrSellProduct(const std::string& productId);
    std::vector<std::string> GetInputs() const;
};