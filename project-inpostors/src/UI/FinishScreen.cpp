#include "UI/FinishScreen.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "UI/Assets/FontAsset.h"
#include "UI/Components/Button.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/UIComponent.h"

#include "UI/UIStyle.h"

FinishScreen::FinishScreen(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {}

std::shared_ptr<FinishScreen> FinishScreen::Create(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent) {
    auto finishScreen = std::shared_ptr<FinishScreen>(
            new FinishScreen(id, name, isStatic, parent));

    finishScreen->CreateBackground();
    finishScreen->CreateLeftPanel();
    finishScreen->CreateRightPanel();

    return finishScreen;
}

void FinishScreen::CreateBackground() {
    background = AddComponent<mlg::Image>(
            "background",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL));

    auto sharedBackground = background.lock();
    sharedBackground->SetSize(PANEL_SIZE);
    sharedBackground->SetPosition(MLG_POS_CENTER);
    sharedBackground->SetAnchor(MLG_ANCHOR_CENTER);

    headerText = AddComponent<mlg::Label>(
            "header",
            mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));
    auto sharedHeaderText = headerText.lock();
    sharedHeaderText->SetSize(50);
    sharedHeaderText->SetText("Time's up!");
    sharedHeaderText->SetPosition(
            MLG_POS_CENTER + glm::vec2{
                                     -PANEL_SIZE.x * 0.47f,
                                     PANEL_SIZE.y * 0.42});
    sharedHeaderText->SetAnchor(MLG_ANCHOR_CENTER);
    sharedHeaderText->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
}

// We need a better ui system D:

void FinishScreen::CreateLeftPanel() {
    leftBackground = AddComponent<mlg::Image>(
            "background",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL));

    auto sharedLeftBackground = leftBackground.lock();
    const glm::vec2 leftSize = glm::vec2{PANEL_SIZE.x * 0.45, PANEL_SIZE.y * 0.8};
    sharedLeftBackground->SetSize(leftSize);
    sharedLeftBackground->SetPosition(
            MLG_POS_CENTER + glm::vec2{
                                     -PANEL_SIZE.x * 0.24,
                                     -PANEL_SIZE.y * 0.05});
    sharedLeftBackground->SetAnchor(MLG_ANCHOR_CENTER);

    exitButton = AddComponent<mlg::Button>(
            "exit_button",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_MATERIAL),
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_FOCUSED_MATERIAL),
            mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));

    auto sharedExitButton = exitButton.lock();
    sharedExitButton->GetLabel().lock()->SetText("Exit");
    sharedExitButton->SetSize(BUTTON_SIZE);
    sharedExitButton->SetPosition(
            MLG_POS_CENTER + glm::vec2{
                                     -PANEL_SIZE.x * 0.24,
                                     -PANEL_SIZE.y * 0.35});
    sharedExitButton->SetAnchor(MLG_ANCHOR_CENTER);

    scoreLabel = AddComponent<mlg::Label>(
            "score",
            mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));
    auto sharedScoreLabel = scoreLabel.lock();
    sharedScoreLabel->SetSize(30);
    sharedScoreLabel->SetText("Score: $2137");
    sharedScoreLabel->SetPosition(
            MLG_POS_CENTER + glm::vec2{
                                     -PANEL_SIZE.x * 0.24,
                                     PANEL_SIZE.y * 0.28});
    sharedScoreLabel->SetAnchor(MLG_ANCHOR_CENTER);
    sharedScoreLabel->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
    sharedScoreLabel->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
}

void FinishScreen::CreateRightPanel() {
    rightBackground = AddComponent<mlg::Image>(
            "background",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL));
    auto sharedRightBackground = rightBackground.lock();

    const glm::vec2 rightSize = glm::vec2{PANEL_SIZE.x * 0.45, PANEL_SIZE.y * 0.8};
    sharedRightBackground->SetSize(rightSize);
    sharedRightBackground->SetPosition(
            MLG_POS_CENTER + glm::vec2{
                                     PANEL_SIZE.x * 0.24,
                                     -PANEL_SIZE.y * 0.05});
    sharedRightBackground->SetAnchor(MLG_ANCHOR_CENTER);

    rightHeaderText = AddComponent<mlg::Label>(
            "header",
            mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));

    auto sharedRightHeaderText = rightHeaderText.lock();
    sharedRightHeaderText->SetSize(30);
    sharedRightHeaderText->SetText("SCOREBOARD");
    sharedRightHeaderText->SetPosition(
            MLG_POS_CENTER + glm::vec2{
                                     PANEL_SIZE.x * 0.24,
                                     PANEL_SIZE.y * 0.28});
    sharedRightHeaderText->SetAnchor(MLG_ANCHOR_CENTER);
    sharedRightHeaderText->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
    sharedRightHeaderText->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);

    auto scoreboard = AddComponent<mlg::Label>(
            "scoreboard",
            mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));

    auto sharedScoreboard = scoreboard.lock();
    sharedScoreboard->SetSize(28);
    sharedScoreboard->SetText("MLG0 $2137\nMLG1 $2137\nMLG2 $2137\nMLG3 $2137\nMLG4 $2137\nMLG5 $2137\nMLG6 $2137\nMLG7 $2137\nMLG8 $2137\nMLG9 $2137\nML10 $2137");

    sharedScoreboard->SetPosition(
            MLG_POS_CENTER + glm::vec2{
                                     PANEL_SIZE.x * 0.24,
                                     PANEL_SIZE.y * 0.16});
    sharedScoreboard->SetAnchor(MLG_ANCHOR_CENTER);
    sharedScoreboard->SetVerticalAlignment(mlg::Label::VerticalAlignment::Up);
    sharedScoreboard->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
}