#pragma once

struct Product {
    std::string name;
    std::shared_ptr<class TextureAsset> icon;
    glm::vec3 color;
};

class ProductManager {
    static ProductManager* instance;

    std::unordered_map<std::string, Product> productMap;
    
    ProductManager() = default;

public:
    static ProductManager* GetInstance();

    const Product& GetProduct(const std::string& name);

private:
    void LoadConfig(const std::string& path);
};
