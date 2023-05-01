#pragma once

#include <Gameplay/Entity.h>

struct PlayerData {
    int id;
    glm::vec4 color;
};

class Player : public mlg::Entity {
private:
    Player(const std::string& name, bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<Player> Create(const std::string& name, bool isStatic, mlg::Transform* parent,
                                          const PlayerData& playerData,
                                          const std::string& configPath = "res/config/cars/testing.json");

    ~Player() = default;

};
