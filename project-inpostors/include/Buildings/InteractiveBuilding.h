#pragma once

#include "Gameplay/Entity.h"

namespace mlg {
    class RigidbodyComponent;
}

class InteractiveBuilding : public mlg::Entity {
public:
    InteractiveBuilding(
            size_t id,
            const std::string& name,
            bool isStatic,
            mlg::Transform* parent);

    virtual bool TakeInputsFromInventory(class EquipmentComponent& equipment) = 0;

protected:
    glm::vec2 meshOffset;

    void AddMesh(const nlohmann::json& staticMeshJson);
    void AddCollider(const nlohmann::json& colliderJson,
                     mlg::RigidbodyComponent* rigidbodyComponent);
    void AddEmitter(const nlohmann::json& emitterJson);
    void AddTrigger(const nlohmann::json& triggerJson,
                    const std::string& triggerName,
                    mlg::RigidbodyComponent* rigidbodyComponent);
};
