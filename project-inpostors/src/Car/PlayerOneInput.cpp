#include "Car/PlayerOneInput.h"

#include "Core/HID/Input.h"

#include "Car/CarInput.h"

glm::vec2 PlayerOneInput::GetMovementInput() {
    glm::vec2 movement;

    movement.y = mlg::Input::GetActionStrength("forward_one");
    movement.y -= mlg::Input::GetActionStrength("backward_one");

    movement.x = mlg::Input::GetActionStrength("right_one");
    movement.x -= mlg::Input::GetActionStrength("left_one");

    return movement;
}

PlayerOneInput::~PlayerOneInput() {

}

PlayerOneInput::PlayerOneInput(const std::weak_ptr<mlg::Entity> &owner, const std::string &name)
: CarInput(owner, name) {}

bool PlayerOneInput::GetPickUpInput() {
    return mlg::Input::IsActionJustPressed("pick_up_one");
}

