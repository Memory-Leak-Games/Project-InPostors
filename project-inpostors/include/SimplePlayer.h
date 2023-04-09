#pragma once

#include <Gameplay/Entity.h>

class SimplePlayer : public mlg::Entity {
private:
    SimplePlayer(const std::string& name, bool isStatic, mlg::Transform* parent);
public:
    static std::shared_ptr<SimplePlayer> Create(const std::string& name, bool isStatic, mlg::Transform* parent);

    ~SimplePlayer() = default;

};
