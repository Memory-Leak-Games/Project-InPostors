#pragma once

namespace mlg {
    class MaterialAsset;
}

struct Product {
    std::string name;
    std::shared_ptr<class mlg::MaterialAsset> icon;
    int price;
    glm::vec3 color;
};

class ProductManager {
    static ProductManager* instance;

    std::unordered_map<std::string, Product> productMap;

    ProductManager() = default;

public:
    static ProductManager* Get();

    const Product& GetProduct(const std::string& name);

private:
    void LoadConfig(const std::string& path);
};
