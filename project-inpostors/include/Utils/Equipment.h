#pragma  once

#include "Gameplay/Component.h"

class Equipment : public mlg::Component {
private:
    int size;
    std::vector<std::shared_ptr<class Product>> equipment;

public:
    Equipment(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, int size);

    eventpp::CallbackList<void()> equipmentChanged;

    bool AddProduct(const std::shared_ptr<Product>& product);
    bool RequestProduct(const std::shared_ptr<Product>& product);
    bool CheckProduct(const std::shared_ptr<Product>& product) const;
    bool CheckBlueprint(const class Blueprint& blueprint) const;
    bool CheckIsFull() const;

    ~Equipment() override;
};

