#pragma  once

#include "Gameplay/Component.h"
#include <string>

class EquipmentComponent : public mlg::Component {
private:
    int size;
    std::vector<std::string> equipment;

public:
    EquipmentComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, int size);

    eventpp::CallbackList<void()> equipmentChanged;

    bool AddProduct(const std::string& productId);
    bool RequestProduct(const std::string& productId);
    std::string RequestProduct();

    bool CheckProduct(const std::string& productId) const;
    bool CheckBlueprint(const std::string& blueprintId) const;
    bool CheckIsFull() const;

    std::string ToString() const;

    ~EquipmentComponent() override;
};

