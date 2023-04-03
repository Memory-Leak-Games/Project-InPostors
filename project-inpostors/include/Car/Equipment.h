#pragma  once

#include <memory>
#include <vector>
#include <eventpp/callbacklist.h>
#include "include/Utils/Product.h"
#include "include/Utils/Blueprint.h"


class Equipment {
private:
    int size;
    std::vector<std::shared_ptr<Product>> equipment;
    eventpp::CallbackList<void()> equipmentChanged;

public:
    bool AddProduct(const std::shared_ptr<Product>& product);
    bool RequestProduct(const std::shared_ptr<Product>& product);
    bool CheckProduct(const std::shared_ptr<Product>& product) const;
    bool CheckBlueprint(const Blueprint& blueprint) const;
    bool CheckIsFull() const;
    ~Equipment() = default;

};

