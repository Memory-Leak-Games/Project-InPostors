#include "include/Utils/Product.h"

bool Product::Equals(Product& another) const {
    return name == another.name &&
            icon == another.icon &&
            color == another.color;
}
