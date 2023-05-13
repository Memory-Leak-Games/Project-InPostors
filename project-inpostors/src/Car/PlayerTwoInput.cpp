#include "Car/PlayerTwoInput.h"

#include "Core/HID/Input.h"

glm::vec2 PlayerTwoInput::GetMovementInput() {
    glm::vec2 movement;

    movement.y = mlg::Input::GetActionStrength("forward_two");
    movement.y -= mlg::Input::GetActionStrength("backward_two");

    movement.x = mlg::Input::GetActionStrength("right_two");
    movement.x -= mlg::Input::GetActionStrength("left_two");

    return movement;
}

PlayerTwoInput::PlayerTwoInput(const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
: CarInput(owner, name) {}

PlayerTwoInput::~PlayerTwoInput() {}

bool PlayerTwoInput::GetPickUpInput() {
    //TODO:
    return false;
}

bool PlayerTwoInput::GetDropInput() {
    //TODO:
    return false;
}
