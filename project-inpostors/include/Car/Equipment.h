#pragma  once

#include <memory>
#include <vector>
#include "include/Utils/Product.h"
#include "include/Utils/Blueprint.h"

class Equipment {
private:
    int size;
    std::vector<std::shared_ptr<Product>> equipment;

public:
    virtual bool AddProduct(const std::shared_ptr<Product>& product) = 0;
    virtual bool RequestProduct(const std::shared_ptr<Product>& product) = 0;
    virtual bool CheckProduct(const std::shared_ptr<Product>& product) = 0;
    virtual bool CheckBlueprint(const Blueprint& blueprint) = 0;
    virtual bool CheckIsFull() = 0;
    virtual ~Equipment() = default;

};

