#pragma once

#include "Gameplay/Entity.h"
#include <memory>

namespace mlg {
    class RigidbodyComponent;
    class Image;
    class Label;
    class ProgressBar;
}

class Factory : public mlg::Entity {
private:
    Factory(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

    std::shared_ptr<class EquipmentComponent> equipmentComponent;


    std::string blueprintId;

    std::shared_ptr<class mlg::Label> amount1;
    std::shared_ptr<class mlg::Label> amount2;
    std::shared_ptr<class mlg::Label> amount3;
    std::shared_ptr<class mlg::ProgressBar> productionBar;

    unsigned int produceTimerHandle = 0;

public:
    ~Factory() override;

    static std::shared_ptr<Factory> Create(uint64_t id, const std::string& name, bool isStatic,
                                           mlg::Transform* parent, const std::string& configPath);

    const std::shared_ptr<EquipmentComponent>& GetEquipmentComponent() const;

    std::string GetBlueprintId() const;

    void Start() override;
    void Update() override;

    bool IsWorking() const;

private:
    enum class FactoryType {
        OneInput,
        OneOutput,
        OneInputOutput,
        SeparateInputOutput
    } factoryType;

    glm::vec2 meshOffset;

    void AddMesh(const nlohmann::json& staticMeshJson);
    void AddCollider(const nlohmann::json& colliderJson, mlg::RigidbodyComponent* rigidbodyComponent);
    void AddEmitter(const nlohmann::json& emitterJson);
    void AddTrigger(const nlohmann::json& triggerJson, const std::string& triggerName,
                    mlg::RigidbodyComponent* rigidbodyComponent);

    void CheckBlueprintAndStartWorking();
    void ProduceItem();
    //void Update() override;

    static void GenerateUI(const std::shared_ptr<Factory>& result);
};
