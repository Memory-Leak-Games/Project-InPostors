#pragma once

#include <string>

class Product {
private:
    std::string name;
    std::string icon;
    int color;

public:
    bool Equals(const std::shared_ptr<Product>& another) const;

    const std::string& GetName() const;
    const std::string& GetIcon() const;
    int GetColor() const;

};