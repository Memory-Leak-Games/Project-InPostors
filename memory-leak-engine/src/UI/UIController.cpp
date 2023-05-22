#include "include/UI/UIController.h"
#include "UI/Components/UIFocusableComponent.h"
#include <Core/HID/Input.h>

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

    UIController* UIController::GetInstance() {
        return instance;
    }

    void UIController::Update() {
        if (instance->focused.expired())
            return;

        auto next = instance->focused.lock()->next;

        if (Input::IsActionJustPressed("ui_up")) {
            if (!next.top.expired()) {
                instance->focused.lock()->hasFocus = false;
                next.top.lock()->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_down")) {
            if (!next.bottom.expired()) {
                instance->focused.lock()->hasFocus = false;
                next.bottom.lock()->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_left")) {
            if (!next.left.expired()) {
                instance->focused.lock()->hasFocus = false;
                next.left.lock()->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_right")) {
            if (!next.right.expired()) {
                instance->focused.lock()->hasFocus = false;
                next.right.lock()->GrabFocus();
            }
        }
    }
}