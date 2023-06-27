#pragma once

#include "Audio/Assets/AudioAsset.h"
#include "Player/PlayerFXComponent.h"
#include <Gameplay/Entity.h>
#include <eventpp/callbacklist.h>
#include <memory>

namespace mlg {
    class RigidbodyComponent;
    class Image;
}// namespace mlg

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

    PlayerData playerData;

    friend class PlayerUI;
private:
    Player(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent,
            const PlayerData& playerData);

public:
    eventpp::CallbackList<void()> OnPickUp;
    eventpp::CallbackList<void()> OnDrop;
    eventpp::CallbackList<void()> OnWrongAction;

    static std::shared_ptr<Player> Create(
            uint64_t id, const std::string& name, bool isStatic,
            mlg::Transform* parent, const PlayerData& playerData);

    void Start() override;
    void Update() override;

    void SetPlayerPosition(const glm::vec2& position);
    void SetPlayerRotation(float angle);

    ~Player() override = default;

    const PlayerData& GetPlayerData() const;

private:
    void AddRigidbody(const nlohmann::json& configJson);
    void LoadModel(const nlohmann::json& configJson);
    void LoadAnimatedModel(const nlohmann::json& configJson);

    bool PickUp();
    bool Drop();
};
