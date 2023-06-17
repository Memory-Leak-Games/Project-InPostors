#include "include/UI/Components/UIFocusableComponent.h"
#include "Gameplay/ComponentManager.h"
#include <UI/UIController.h>

namespace mlg {
    UIFocusableComponent::UIFocusableComponent(std::weak_ptr<Entity> owner, std::string name) : UIComponent(owner, name), next() {}

    void UIFocusableComponent::GrabFocus() {
        if (!active)
            return;

        auto sharedThis = std::dynamic_pointer_cast<UIFocusableComponent>(
                ComponentManager::GetByRawPointer(this).lock());

        if (sharedThis == nullptr)
            return;

        UIController::Get()->SetFocus(sharedThis);
    }

    void UIFocusableComponent::SetActive(bool active) {
        this->active = active;
    }

    bool UIFocusableComponent::GetActive() const {
        return active;
    }

    bool UIFocusableComponent::IsFocused() const {
        return hasFocus;
    }
}// namespace mlg