#pragma  once

#include "Gameplay/Component.h"
#include <string>
#include <vector>

class EquipmentComponent : public mlg::Component {
private:
    int size;
    std::vector<std::string> equipment;

public:
    EquipmentComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, int size);
    ~EquipmentComponent() override;

    eventpp::CallbackList<void()> equipmentChanged;
    eventpp::CallbackList<void()> productAdded;
    eventpp::CallbackList<void()> productRemoved;

    bool AddProduct(const std::string& productId);
    bool RequestProduct(const std::string& productId);
    std::string RequestOldestProduct();

    std::string RequestProduct();
    const std::vector<std::string>& GetProducts() const;

    bool Has(const std::string& productId) const;
    int GetNumberOfProduct(const std::string& productId) const;
    bool IsFull() const;
    bool IsEmpty() const;
    const std::vector<std::string> GetEquipment() const;

    int GetMaxSize();

    std::string ToString() const;
};

