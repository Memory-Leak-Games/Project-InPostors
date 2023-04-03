#include "include/Utils/Product.h"

bool Product::Equals(const std::shared_ptr<Product>& another) const {
    return name == another->name &&
            icon == another->icon &&
            color == another->color;
}
