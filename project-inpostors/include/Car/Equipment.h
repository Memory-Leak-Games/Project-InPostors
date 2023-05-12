#pragma  once

#include "Gameplay/Component.h"

class Equipment : mlg::Component {
private:
    int size;
    std::vector<std::shared_ptr<class Product>> equipment;

public:
    eventpp::CallbackList<void()> equipmentChanged;

    bool AddProduct(const std::shared_ptr<Product>& product);
    bool RequestProduct(const std::shared_ptr<Product>& product);
    bool CheckProduct(const std::shared_ptr<Product>& product) const;
    bool CheckBlueprint(const class Blueprint& blueprint) const;
    bool CheckIsFull() const;

    ~Equipment() override;
};

