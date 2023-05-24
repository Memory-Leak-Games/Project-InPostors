#include <Utils/Blueprint.h>
#include <Utils/EquipmentComponent.h>
#include <math.h>
#include <string>

Blueprint::Blueprint(const nlohmann::json& blueprintJson)
    : name(blueprintJson["name"]), input(), output(blueprintJson["output"]),
     timeToProcess(blueprintJson["time"]) {
    for (const auto& inputJson : blueprintJson["input"]) {
        input.push_back(inputJson);
    }
}

float Blueprint::GetTimeToProcess() const {
    return timeToProcess;
}

std::string Blueprint::GetOutput() const {
    return output;
}

const std::vector<std::string>& Blueprint::GetInput() const {
    return input;
}

bool Blueprint::CheckBlueprint(const EquipmentComponent& component) const {
    if (input.empty())
        return true;

    for (const auto& item : input) {
        if (!component.Has(item))
            return false;
    }

    return true;
}

