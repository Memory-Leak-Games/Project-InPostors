#pragma once

#include "Gameplay/Entity.h"

namespace mlg {
    class RigidbodyComponent;
    class Image;
    class Label;
    class ProgressBar;
    class AudioAsset;
}

class Factory : public mlg::Entity {
private:
    Factory(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

    std::shared_ptr<mlg::RigidbodyComponent> mainRigidbody;

    std::shared_ptr<mlg::AudioAsset> createProductSound;
    std::string blueprintId;
    std::shared_ptr<class FactoryEquipmentComponent> factoryEquipment;

    std::shared_ptr<class mlg::ProgressBar> barReq1;
    std::shared_ptr<class mlg::ProgressBar> barReq2;
    std::shared_ptr<class mlg::ProgressBar> barArrow;
    std::shared_ptr<class mlg::ProgressBar> barRes;

    unsigned int produceTimerHandle = 0;
    bool working = false;

public:
    ~Factory() override;

    static std::shared_ptr<Factory> Create(uint64_t id, const std::string& name, bool isStatic,
                                           mlg::Transform* parent, const std::string& configPath);

    std::string GetBlueprintId() const;

    void Start() override;
    void Update() override;

    bool IsWorking() const;

    const std::vector<std::string> GetInputs() const;

    bool TakeInputsFromInventory(class EquipmentComponent& equipment);
    std::string GiveOutput();

private:
    enum class FactoryType {
        OneInput,
        OneOutput,
        OneInputOutput,
        SeparateInputOutput,
        Storage
    } factoryType;

    glm::vec2 meshOffset;

    void AddMesh(const nlohmann::json& staticMeshJson);
    void AddCollider(const nlohmann::json& colliderJson, mlg::RigidbodyComponent* rigidbodyComponent);
    void AddEmitter(const nlohmann::json& emitterJson);
    void AddTriggers(const nlohmann::json& config);
    void AddTrigger(const nlohmann::json& triggerJson, const std::string& triggerName,
                    mlg::RigidbodyComponent* rigidbodyComponent);

    void CheckBlueprintAndStartWorking();
    void ProduceItem();

    void FinishTask();

    static void GenerateUI(const std::shared_ptr<Factory>& result);
    void UpdateUi();

    void StartAsFactory();
    void StartAsStorage();

    bool TakeInputsAsStorage(EquipmentComponent& equipment);
    bool TakeInputsAsFactory(EquipmentComponent& equipment);

    bool CheckBlueprint();
};
