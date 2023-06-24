#pragma once

#include "Gameplay/Entity.h"
#include <eventpp/callbacklist.h>
#include <memory>
#include <unordered_map>

#define TUTORIAL_CONFIG_PATH "res/levels/tutorial.json"

namespace mlg {
    class CanvasPanel;
    class Button;
    class Label;
    class Image;
}// namespace mlg

class TutorialPanel : public mlg::Entity {
private:
    std::shared_ptr<mlg::CanvasPanel> panel;
    std::shared_ptr<mlg::Button> exitButton;
    std::shared_ptr<mlg::Label> text;

    std::unordered_map<std::string, std::string> messages;

    TutorialPanel(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);

public:
    eventpp::CallbackList<void()> OnClosed;
    eventpp::CallbackList<void()> OnMessage;

    static std::shared_ptr<TutorialPanel> Create(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);
    ~TutorialPanel() override;
    
    void ShowTutorial(const std::string& messageId);

private:
    void SetVisible(bool visible);
    void ParseJson();
};
