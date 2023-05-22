#include "include/UI/Components/UIFocusableComponent.h"
#include <UI/UIController.h>

namespace mlg {
    UIFocusableComponent::UIFocusableComponent(std::weak_ptr<Entity> owner, std::string name) : UIComponent(owner, name), next(nullptr, nullptr, nullptr, nullptr) {}

    void UIFocusableComponent::GrabFocus() {
        hasFocus = true;
        UIController::GetInstance()->focused = this;
    }
}