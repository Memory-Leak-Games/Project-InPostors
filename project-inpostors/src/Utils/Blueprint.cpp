#include <Utils/Blueprint.h>
#include <Utils/EquipmentComponent.h>

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

bool Blueprint::CheckBlueprint(const EquipmentComponent& component) const {
    if (input.empty())
        return true;

    // todo: implement me
    return false;
}

