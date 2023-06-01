#pragma once

#include <Gameplay/Entity.h>
#include "Audio/Assets/AudioAsset.h"

namespace mlg {
    class RigidbodyComponent;
}

struct PlayerData {
    int id;
    glm::vec4 color;
    glm::vec3 initialPosition = {0.f, 0.3f, 0.f};
    float initialRotation;
    std::string carData;
};

class Player : public mlg::Entity {
private:

    std::weak_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::shared_ptr<class CarInput> carInput;
    std::shared_ptr<class EquipmentComponent> equipment;

    std::shared_ptr<class mlg::AudioAsset> pickUpSound;
    std::shared_ptr<class mlg::AudioAsset> dropSound;
    std::shared_ptr<class mlg::AudioAsset> hitSound;

    bool canPlaySound = true;
    unsigned int canPlaySoundTimerHandle = 0;

    PlayerData playerData;

private:
    Player(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent, const PlayerData& playerData);

public:
    static std::shared_ptr<Player> Create(uint64_t id, const std::string& name, bool isStatic,
                                          mlg::Transform* parent, const PlayerData& playerData);

    void Start() override;
    void Update() override;

    ~Player() override = default;

private:
    void AddRigidbody(const nlohmann::json& configJson, const float rotation);
    void LoadModel(const nlohmann::json& configJson);

    void PickUp();
    void Drop();
};
