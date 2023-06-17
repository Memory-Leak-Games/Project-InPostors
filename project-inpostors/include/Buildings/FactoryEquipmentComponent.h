#pragma once

#include "Gameplay/Component.h"

class FactoryEquipmentComponent : public mlg::Component {
private:
    // productId, isPresent
    std::unordered_map<std::string, bool> inputs;
    std::pair<std::string, bool> output;

public:
    eventpp::CallbackList<void()> inputProductAdded;
    eventpp::CallbackList<void()> outputProductRemoved;

    FactoryEquipmentComponent(
            const std::weak_ptr<mlg::Entity>& owner,
            const std::string& name,
            const std::string& blueprintId);

    ~FactoryEquipmentComponent() override;

    void Produce();

    void AddInput(const std::string& productId);
    void ClearInput();
    void RemoveOutput();

    bool IsAllInputsPresent() const;
    bool IsOutputPresent() const;
    bool CanStoreInput(const std::string& productId) const;
    bool Has(const std::string& productId) const;
};
