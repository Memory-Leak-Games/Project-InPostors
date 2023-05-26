#pragma once

#include <string>
#include <vector>
class Blueprint {
    std::string name;
    std::vector<std::string> input;
    std::string output;
    float timeToProcess;

public:
    explicit Blueprint(const nlohmann::json& blueprintJson);

    bool CheckBlueprint(const class EquipmentComponent& component) const;

    std::string GetOutput() const;
    const std::vector<std::string>& GetInput() const;
    float GetTimeToProcess() const;
};