#pragma once

#include "Gameplay/Entity.h"

namespace mlg {
    class RigidbodyComponent;
}

class Factory : public mlg::Entity {
private:
    Factory(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<Factory> Create(uint64_t id, const std::string& name, bool isStatic,
                                           mlg::Transform* parent, const std::string& configPath);

private:
    enum class FactoryType {
        OneInput,
        OneOutput,
        OneInputOutput,
        SeparateInputOutput
    } factoryType;

    void AddMesh(const nlohmann::json& staticMeshJson);
    void AddCollider(const nlohmann::json& colliderJson, mlg::RigidbodyComponent* rigidbodyComponent);
    void AddEmitter(const nlohmann::json& emitterJson);
    void AddTrigger(const nlohmann::json& triggerJson, const std::string& triggerName,
                    mlg::RigidbodyComponent* rigidbodyComponent);
};
