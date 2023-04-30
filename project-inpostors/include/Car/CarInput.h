#pragma once

#include "Gameplay/Component.h"

#include "glm/vec2.hpp"

class CarInput : public mlg::Component {
public:
    CarInput(const std::weak_ptr<mlg::Entity> &owner, const std::string &name) : Component(owner, name) {}

    virtual glm::vec2 GetMovementInput() = 0;
};
