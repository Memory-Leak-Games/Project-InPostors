#pragma once

#include <Gameplay/Entity.h>

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

private:
    Player(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<Player> Create(uint64_t id, const std::string& name, bool isStatic,
                                          mlg::Transform* parent, const PlayerData& playerData);

    void Start() override;
    void Update() override;

    ~Player() = default;

private:
    void PickUp();
};
