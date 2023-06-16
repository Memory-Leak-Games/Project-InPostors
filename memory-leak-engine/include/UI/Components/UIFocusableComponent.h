#pragma once

#include "UIComponent.h"
#include <glm/fwd.hpp>

namespace mlg {
    class UIFocusableComponent : public UIComponent {
    private:
        bool active = true;

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

        void SetActive(bool active);
        bool GetActive() const;

    };

}