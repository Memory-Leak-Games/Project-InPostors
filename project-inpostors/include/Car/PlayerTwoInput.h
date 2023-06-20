#pragma once

#include "CarInput.h"

class PlayerTwoInput : public CarInput {
public:
    PlayerTwoInput(const std::weak_ptr<mlg::Entity>& owner, const std::string& name);
    ~PlayerTwoInput() override;

    glm::vec2 GetMovementInput() override;

    bool GetPickUpInput() override;
};
