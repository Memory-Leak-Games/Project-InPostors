#include "UI/TutorialPanel.h"

#include <fstream>

#include "Core/AssetManager/AssetManager.h"
#include "Core/Time.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Builders/ButtonBuilder.h"
#include "UI/Builders/LabelBuilder.h"
#include "UI/Components/Button.h"
#include "UI/Components/Containers/CanvasPanel.h"

#include "Managers/ChatManager.h"

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

    auto backgroundMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL);

    auto background = panel->AddComponent<mlg::Image>(
                                   "background", backgroundMaterial)
                              .lock();
    background->SetSize(PANEL_SIZE);

    mlg::ButtonBuilder buttonBuilder;
    panel->exitButton = buttonBuilder.SetName("ExitButton")
                                .SetText("Hide")
                                .BuildButton(panel.get())
                                .lock();
    panel->exitButton->SetRelativePosition({0.f, -PANEL_SIZE.y / 2.f + 60.f});

    panel->exitButton->OnClick.append(
            [panel]() {
                panel->SetVisible(false);
                mlg::Time::PauseGame(false);
                panel->OnClosed();
            });

    mlg::LabelBuilder labelBuilder;
    auto title = labelBuilder.SetName("Title")
                         .SetText("You have a brand new message from AiPost!")
                         .SetSize(40.f)
                         .SetHorizontalAlignment(MLG_H_ALIGN_CENTER)
                         .BuildLabel(panel.get());

    title.lock()->SetRelativePosition({0.f, PANEL_SIZE.y / 2.f - 50.f});

    panel->text = labelBuilder.SetName("Text")
                          .SetText("PLACEHOLDER")
                          .SetSize(28.f)
                          .SetHorizontalAlignment(MLG_H_ALIGN_LEFT)
                          .BuildLabel(panel.get())
                          .lock();

    panel->text->SetRelativePosition(
            {-PANEL_SIZE.x / 2.f + 20, PANEL_SIZE.y / 2.f - 90.f});

    panel->panel = panel->AddComponent<mlg::CanvasPanel>("TutorialCanvas").lock();
    panel->panel->SetSize(PANEL_SIZE);
    panel->panel->AddChild(background);
    panel->panel->AddChild(panel->exitButton);
    panel->panel->AddChild(title);
    panel->panel->AddChild(panel->text);

    panel->panel->SetAnchor(MLG_ANCHOR_CENTER);
    panel->panel->SetPosition(MLG_POS_CENTER);

    return panel;
}

TutorialPanel::~TutorialPanel() = default;

void TutorialPanel::ShowTutorial(const std::string& messageId) {
    SetVisible(true);
    std::string wrappedMessage = mlg::Label::WrapText(
            messages[messageId], 66);
    text->SetText(wrappedMessage);
    mlg::Time::PauseGame(true);
}

void TutorialPanel::SetVisible(bool visible) {
    panel->SetVisible(visible);
    exitButton->GrabFocus();
}

void TutorialPanel::ParseJson() {
    std::ifstream file{TUTORIAL_CONFIG_PATH};
    json tutorialJson = json::parse(file);
    file.close();

    for (const auto& [key, value] : tutorialJson.items()) {
        messages.insert({key, value});
    }
}
