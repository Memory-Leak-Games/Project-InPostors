#pragma once

#include <Gameplay/Entity.h>

namespace mlg {
    class RigidbodyComponent;
}

struct PlayerData {
    int id;
    glm::vec4 color;
};

class Player : public mlg::Entity {
private:
    std::weak_ptr<mlg::RigidbodyComponent> rigidbodyComponent;

private:
    Player(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<Player> Create(uint64_t id, const std::string& name, bool isStatic,
                                          mlg::Transform* parent, const PlayerData& playerData,
                                          const std::string& configPath = "res/config/cars/testing.json");

    void Update() override;

    ~Player() = default;

};
