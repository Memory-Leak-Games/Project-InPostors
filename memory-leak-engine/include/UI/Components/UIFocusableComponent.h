#pragma once

#include "UIComponent.h"

namespace mlg {
    class UIFocusableComponent : public UIComponent {
    public:
        struct Next {
            std::weak_ptr<UIFocusableComponent> top;
            std::weak_ptr<UIFocusableComponent> bottom;
            std::weak_ptr<UIFocusableComponent> left;
            std::weak_ptr<UIFocusableComponent> right;
        } next;
        bool hasFocus = false;

        UIFocusableComponent(std::weak_ptr<Entity> owner, std::string name);

        virtual void GrabFocus();
    };
}