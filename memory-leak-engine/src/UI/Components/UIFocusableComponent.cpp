#include "include/UI/Components/UIFocusableComponent.h"
#include "Gameplay/ComponentManager.h"
#include <UI/UIController.h>

namespace mlg {
    UIFocusableComponent::UIFocusableComponent(std::weak_ptr<Entity> owner, std::string name) : UIComponent(owner, name), next() {}

    void UIFocusableComponent::GrabFocus() {
        hasFocus = true;
        UIController::GetInstance()->focused = std::dynamic_pointer_cast<UIFocusableComponent>(
                ComponentManager::GetByRawPointer(this).lock());
    }
}