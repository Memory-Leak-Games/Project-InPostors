#pragma once

#include <string>

class Product {
private:
    std::string name;
    std::string icon;
    int color;

public:
    bool Equals(Product &another) const;

};