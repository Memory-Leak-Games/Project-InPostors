#include "Utils/ProductManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define CONFIG_FILE "res/config/products.json"

ProductManager* ProductManager::instance;

ProductManager* ProductManager::Get() {
    if (instance) {
        return instance;
    }

    instance = new ProductManager();
    instance->LoadConfig(CONFIG_FILE);

    return instance;
}

const Product& ProductManager::GetProduct(const std::string& name) {
    return productMap.at(name);
}

void ProductManager::LoadConfig(const std::string& path) {
    std::ifstream configFile(path);
    json configJson = json::parse(configFile);

    for (const auto& productJson : configJson) {
        Product newProduct;

        newProduct.name = productJson["name"];
        newProduct.icon = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(productJson["icon"]);
        newProduct.color = {
                productJson["color"][0],
                productJson["color"][1],
                productJson["color"][2]};
        newProduct.price = productJson["price"];

        productMap[newProduct.name] = newProduct;
    }
}
