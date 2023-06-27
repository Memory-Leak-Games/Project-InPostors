#include "include/UI/UIController.h"

#include "Core/HID/Input.h"
#include "UI/Components/UIFocusableComponent.h"

namespace mlg {

    UIController* UIController::instance;

    UIController::UIController() {
    }

    void UIController::Initialize() {
        SPDLOG_INFO("Initializing UIController");
        if (instance != nullptr)
            return;

        instance = new UIController();
    }

    void UIController::Stop() {
        SPDLOG_INFO("Stopping UIController");

        delete instance;
        instance = nullptr;
    }

    UIController* UIController::Get() {
        return instance;
    }

    void UIController::Update() {
        if (focused.expired())
            return;

        auto next = focused.lock()->next;

        if (Input::IsActionJustPressed("ui_up")) {
            if (!next.top.expired()) {
                focused.lock()->hasFocus = false;
                next.top.lock()->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_down")) {
            if (!next.bottom.expired()) {
                focused.lock()->hasFocus = false;
                next.bottom.lock()->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_left")) {
            if (!next.left.expired()) {
                focused.lock()->hasFocus = false;
                next.left.lock()->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_right")) {
            if (!next.right.expired()) {
                focused.lock()->hasFocus = false;
                next.right.lock()->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_accept")) {
            if (focused.lock()->IsVisible())
                focused.lock()->Accept();
        }
    }

    void UIController::SetFocus(const std::shared_ptr<UIFocusableComponent>& component) {
        if (!component->GetActive() || component->IsFocused())
            return;

        if (!focused.expired()) {
            focused.lock()->hasFocus = false;
            focused.lock()->OnFocusLost();
        }

        component->hasFocus = true;
        component->OnFocus();
        focused = component;
    }
}// namespace mlg