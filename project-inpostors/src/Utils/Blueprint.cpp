#include <Utils/Blueprint.h>
#include <Utils/EquipmentComponent.h>

Blueprint::Blueprint(const nlohmann::json& blueprintJson)
    : name(blueprintJson["name"]), input(), output(blueprintJson["output"]),
     timeToProcess(blueprintJson["timeToProcess"]) {
    for (const auto& inputJson : blueprintJson["input"]) {
        input.push_back(inputJson);
    }
}

bool Blueprint::CheckBlueprint(const EquipmentComponent& component) {
    if (input.empty())
        return true;

    MLG_UNIMPLEMENTED;
    return false;
}
