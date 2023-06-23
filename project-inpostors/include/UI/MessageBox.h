#pragma once

#include "Gameplay/Entity.h"

namespace mlg {
    class CanvasPanel;
    class Label;
}

class MessageBox : public mlg::Entity {
private:
    std::shared_ptr<mlg::CanvasPanel> panel;
    std::shared_ptr<mlg::Label> titleLabel;
    std::shared_ptr<mlg::Label> textLabel;

    MessageBox(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);

public:
    eventpp::CallbackList<void()> OnMessageBoxShow;
    eventpp::CallbackList<void()> OnMessageBoxHide;

    static std::shared_ptr<MessageBox> Create(
            uint64_t id,
            const std::string& name,
            bool isStatic,
            mlg::Transform* parent);

    ~MessageBox() override;

    void ShowMessage(const std::string& title, const std::string& message);
};