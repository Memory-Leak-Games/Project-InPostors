#pragma once

#include <Gameplay/Entity.h>
#include <memory>
#include "Audio/Assets/AudioAsset.h"
#include "Player/PlayerFXComponent.h"

namespace mlg {
    class RigidbodyComponent;
    class Image;
}

struct PlayerData {
    int id;
    glm::vec4 color;
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

    PlayerData playerData;

    friend class PlayerUI;

private:
    Player(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent, const PlayerData& playerData);

public:
    static std::shared_ptr<Player> Create(uint64_t id, const std::string& name, bool isStatic,
                                          mlg::Transform* parent, const PlayerData& playerData);

    void Start() override;
    void Update() override;

    void SetPlayerPosition(const glm::vec2& position);
    void SetPlayerRotation(float angle);

    ~Player() override = default;

private:
    void AddRigidbody(const nlohmann::json& configJson);
    void LoadModel(const nlohmann::json& configJson);

    bool PickUp();
    bool Drop();
	
    //static void GenerateUI(const std::shared_ptr<Player>& newPlayer);
};
