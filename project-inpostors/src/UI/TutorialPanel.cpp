#include "UI/TutorialPanel.h"

#include <eventpp/callbacklist.h>
#include <fstream>

#include "Core/AssetManager/AssetManager.h"
#include "Core/Time.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Builders/ButtonBuilder.h"
#include "UI/Builders/LabelBuilder.h"
#include "UI/Components/Button.h"
#include "UI/Components/Containers/CanvasPanel.h"

#include "Managers/ChatManager.h"

#include "UI/Components/DynamicImage.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/UIComponent.h"
#include "UI/UIStyle.h"

using json = nlohmann::json;

TutorialPanel::TutorialPanel(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {
    ParseJson();
}

std::shared_ptr<TutorialPanel> TutorialPanel::Create(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent) {
    auto panel = std::shared_ptr<TutorialPanel>(
            new TutorialPanel(id, name, isStatic, parent));

    auto logoMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(AI_LOGO_MATERIAL);
    auto image = panel->AddComponent<mlg::Image>("Logo", logoMaterial).lock();
    image->SetSize({512.f, 512.f});

    auto backgroundMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL);

    auto background = panel->AddComponent<mlg::Image>(
                                   "background", backgroundMaterial)
                              .lock();
    background->SetSize(glm::vec2{520, 640});

    mlg::ButtonBuilder buttonBuilder;
    panel->exitButton = buttonBuilder.SetName("ExitButton")
                                .SetText("Hide")
                                .BuildButton(panel.get())
                                .lock();
    panel->exitButton->SetRelativePosition({0.f, -640 / 2.f + 60.f});

    panel->nextButton = buttonBuilder.SetName("NextButton")
                                .SetText("Next")
                                .BuildButton(panel.get())
                                .lock();
    panel->nextButton->SetRelativePosition({0.f, -640 / 2.f + 60.f});

    panel->exitButton->OnClick.append(
            [panel]() {
                panel->SetVisible(false);
                mlg::Time::PauseGame(false);
                panel->OnClosed();
            });

    panel->image = panel->AddComponent<mlg::DynamicImage>("Image").lock();
    panel->image->SetSize({455, 256});
    panel->image->SetRelativePosition({0.f, 125.f});

    mlg::LabelBuilder labelBuilder;
    auto title = labelBuilder.SetName("Title")
                         .SetText("NEW MESSAGE!")
                         .SetSize(40.f)
                         .SetHorizontalAlignment(MLG_H_ALIGN_CENTER)
                         .BuildLabel(panel.get());

    title.lock()->SetRelativePosition({0.f, 640 / 2.f - 50.f});

    panel->text = labelBuilder.SetName("Text")
                          .SetText("PLACEHOLDER")
                          .SetSize(20.f)
                          .SetHorizontalAlignment(MLG_H_ALIGN_LEFT)
                          .BuildLabel(panel.get())
                          .lock();

    panel->text->SetRelativePosition(
            {-540.f / 2.f + 30.f, -35.f});

    panel->panel = panel->AddComponent<mlg::CanvasPanel>("TutorialCanvas").lock();
    panel->panel->SetSize(PANEL_SIZE);
    panel->panel->AddChild(image);
    panel->panel->AddChild(background);
    panel->panel->AddChild(panel->exitButton);
    panel->panel->AddChild(panel->nextButton);
    panel->panel->AddChild(title);
    panel->panel->AddChild(panel->text);
    panel->panel->AddChild(panel->image);

    panel->panel->SetAnchor(MLG_ANCHOR_CENTER);
    panel->panel->SetPosition(MLG_POS_CENTER);

    return panel;
}

TutorialPanel::~TutorialPanel() = default;

void TutorialPanel::ShowTutorial(const std::string& messageId) {
    SetVisible(true);
    ShowPage(messageId);
    nextButton->SetVisible(false);
    exitButton->SetVisible(true);
    OnMessage();
}

void TutorialPanel::ShowTutorials(const std::vector<std::string>& messageIds) {
    if (messageIds.empty()) {
        return;
    } else if (messageIds.size() == 1) {
        ShowTutorial(messageIds[0]);
        return;
    }

    pageNumber = 0;
    SetVisible(true);
    nextButton->SetVisible(true);
    nextButton->GrabFocus();

    exitButton->SetVisible(false);

    eventpp::CallbackList<void()>::Handle onClickHandle;

    onClickHandle = nextButton->OnClick.append(
            [this, messageIds, onClickHandle]() {
                if (pageNumber == messageIds.size() - 1) {
                    nextButton->SetVisible(false);
                    exitButton->SetVisible(true);
                    exitButton->GrabFocus();
                    nextButton->OnClick.remove(onClickHandle);
                }

                ShowPage(messageIds[pageNumber]);
                pageNumber++;
            });
    nextButton->OnClick();

    OnMessage();
}

void TutorialPanel::ShowPage(const std::string& messageId) {
    std::string wrappedMessage = mlg::Label::WrapText(
            messages[messageId].text, 49);

    text->SetText(wrappedMessage);
    image->SetTexture(messages[messageId].image);
    mlg::Time::PauseGame(true);
}

void TutorialPanel::SetVisible(bool visible) {
    panel->SetVisible(visible);
    if (visible)
        exitButton->GrabFocus();
}

void TutorialPanel::ParseJson() {
    std::ifstream file{TUTORIAL_CONFIG_PATH};
    json tutorialJson = json::parse(file);
    file.close();

    for (const auto& [key, value] : tutorialJson.items()) {
        auto image =
                mlg::AssetManager::GetAsset<mlg::TextureAsset>(
                        value["image"].get<std::string>());

        messages.insert(
                {key,
                 {value["text"],
                  image}});
    }
}
