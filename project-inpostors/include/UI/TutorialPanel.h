#pragma once

#include "Gameplay/Entity.h"

#define TUTORIAL_CONFIG_PATH "res/levels/tutorial.json"

namespace mlg {
    class CanvasPanel;
    class Button;
    class Label;
    class Image;
    class DynamicImage;
    class TextureAsset;
}// namespace mlg

class TutorialPanel : public mlg::Entity {
private:
    std::shared_ptr<mlg::CanvasPanel> panel;
    std::shared_ptr<mlg::Button> exitButton;
    std::shared_ptr<mlg::Button> nextButton;
    std::shared_ptr<mlg::Label> text;
    std::shared_ptr<mlg::DynamicImage> image;

    struct Message {
        std::string text;
        std::shared_ptr<mlg::TextureAsset> image;
    };

    std::unordered_map<std::string, Message> messages;

    TutorialPanel(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);

    int pageNumber = 0;
public:
    eventpp::CallbackList<void()> OnClosed;
    eventpp::CallbackList<void()> OnMessage;

    static std::shared_ptr<TutorialPanel> Create(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);
    ~TutorialPanel() override;
    
    void ShowTutorial(const std::string& messageId);
    void ShowTutorials(const std::vector<std::string>& messageIds);

private:
    void SetVisible(bool visible);
    void ParseJson();

    void ShowPage(const std::string& messageId);
};
