#pragma once

#include "Buildings/InteractiveBuilding.h"

namespace mlg {
    class RigidbodyComponent;
    class Image;
    class Label;
    class ProgressBar;
    class AudioAsset;
    class ParticleSystemComponent;
}// namespace mlg

class FactoryAnimationComponent;

class Factory : public InteractiveBuilding {
private:
    Factory(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

    std::shared_ptr<mlg::RigidbodyComponent> mainRigidbody;
    std::shared_ptr<FactoryAnimationComponent> animComponent;

    std::shared_ptr<mlg::AudioAsset> createProductSound;
    std::string blueprintId;
    std::shared_ptr<class FactoryEquipmentComponent> factoryEquipment;

    std::shared_ptr<class FactoryUI> factoryUi;

    std::vector<std::weak_ptr<mlg::ParticleSystemComponent>> emitters;

    unsigned int produceTimerHandle = 0;
    bool working = false;

    friend class FactoryUI;
    friend class FactoryAnimationComponent;

public:
    ~Factory() override;

    static std::shared_ptr<Factory> Create(
            uint64_t id, const std::string& name, bool isStatic,
            mlg::Transform* parent, const std::string& configPath);

    std::string GetBlueprintId() const;

    void Start() override;
    void Update() override;

    bool IsWorking() const;

    const std::vector<std::string> GetInputs() const;

    bool TakeInputsFromInventory(class EquipmentComponent& equipment) override;
    std::string GiveOutput();

    bool IsOutputPresent() const;

private:
    enum class FactoryType {
        OneInput,
        OneOutput,
        OneInputOutput,
        SeparateInputOutput,
    } factoryType;

    glm::vec2 meshOffset;

    void AddTriggers(const nlohmann::json& config);

    void CheckBlueprintAndStartWorking();
    void ProduceItem();

    void FinishTask();
    bool CheckBlueprint();

    void GetAllSmokes();
};
