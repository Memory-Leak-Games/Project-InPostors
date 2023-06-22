#pragma once

#include "CarInput.h"

class PlayerOneInput : public CarInput {
public:
    PlayerOneInput() = delete;

    PlayerOneInput(const std::weak_ptr<mlg::Entity> &owner, const std::string &name);
    ~PlayerOneInput() override;

    glm::vec2 GetMovementInput() override;
    void SetVibration(float lowStrength, float highStrength, int duration) override;

    bool GetPickUpInput() override;
};
