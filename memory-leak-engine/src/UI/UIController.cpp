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
        if (!instance->focused)
            return;

        if (Input::IsActionJustPressed("ui_up")) {
            if (instance->focused->next.top) {
                instance->focused->next.top->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_down")) {
            if (instance->focused->next.bottom) {
                instance->focused->next.bottom->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_left")) {
            if (instance->focused->next.left) {
                instance->focused->next.left->GrabFocus();
            }
        } else if (Input::IsActionJustPressed("ui_right")) {
            if (instance->focused->next.right) {
                instance->focused->next.right->GrabFocus();
            }
        }
    }
}