#include "include/Utils/Product.h"

bool Product::Equals(const std::shared_ptr<Product>& another) const {
    return name == another->name &&
            icon == another->icon &&
            color == another->color;
}

const std::string& Product::GetName() const {
    return name;
}

const std::string& Product::GetIcon() const {
    return icon;
}

int Product::GetColor() const {
    return color;
}
