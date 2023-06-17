#pragma once

#include "UI/UIController.h"
#include "UIComponent.h"
#include <glm/fwd.hpp>

namespace mlg {
    class UIController;
    class UIFocusableComponent : public UIComponent {
    private:
        bool active = true;

        bool hasFocus = false;
    public:
        struct Next {
            std::weak_ptr<UIFocusableComponent> top;
            std::weak_ptr<UIFocusableComponent> bottom;
            std::weak_ptr<UIFocusableComponent> left;
            std::weak_ptr<UIFocusableComponent> right;
        } next;

        UIFocusableComponent(std::weak_ptr<Entity> owner, std::string name);

        eventpp::CallbackList<void()> OnFocus;
        eventpp::CallbackList<void()> OnFocusLost;

        virtual void GrabFocus();
        virtual void Accept() = 0;

        void SetActive(bool active);
        bool GetActive() const;

        bool IsFocused() const;

        friend mlg::UIController;
    };

}