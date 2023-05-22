#pragma once

#include "UIComponent.h"

namespace mlg {
    class UIFocusableComponent : public UIComponent {
    public:
        struct Next {
            UIFocusableComponent* top;
            UIFocusableComponent* bottom;
            UIFocusableComponent* left;
            UIFocusableComponent* right;
        } next;
        bool hasFocus = false;

        UIFocusableComponent(std::weak_ptr<Entity> owner, std::string name);

        virtual void GrabFocus();
    };
}