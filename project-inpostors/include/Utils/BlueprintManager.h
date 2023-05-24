#pragma once

#include "Utils/Blueprint.h"

class BlueprintManager {
    static BlueprintManager* instance;

    std::unordered_map<std::string, Blueprint> blueprintMap;
    
    BlueprintManager() = default;

public:
    static BlueprintManager* GetInstance();

    const Blueprint& GetBlueprint(const std::string& name);

private:
    void LoadConfig(const std::string& path);
};