#include "Utils/BlueprintManager.h"

#include <fstream>

#define CONFIG_FILE "res/config/blueprints.json"

BlueprintManager* BlueprintManager::instance;

using json = nlohmann::json;

BlueprintManager* BlueprintManager::Get() {
    
    if (instance) {
        return instance;
    }

    instance = new BlueprintManager();
    instance->LoadConfig(CONFIG_FILE);

    return instance;
}

const Blueprint& BlueprintManager::GetBlueprint(const std::string& name) {
    return blueprintMap.at(name);
}

void BlueprintManager::LoadConfig(const std::string& path) {
    std::ifstream configFile(path);
    json configJson = json::parse(configFile);

    for (const auto& blueprintJson : configJson) {
        blueprintMap.emplace(blueprintJson["name"], blueprintJson);
    }
}

