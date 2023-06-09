#include "UI/FinishScreen.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/SceneManager/SceneManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "ScoreManager.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/Button.h"
#include "UI/Components/CharacterSelector.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/UIComponent.h"

#include "UI/UIStyle.h"

#include "Scenes/LevelScene.h"
#include "ScoreManager.h"

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

FinishScreen::~FinishScreen() = default;

void FinishScreen::Start() {
    exitButton.lock()->OnClick.append([this]() {
        auto currentScene = mlg::SceneManager::GetCurrentScene();
        auto levelScene = dynamic_cast<LevelScene*>(currentScene);

        int score = levelScene->GetScoreManager()->GetScore();
        std::string levelName = levelScene->GetLevelName();
        std::string playerName = GetUserName();

        levelScene->GetScoreManager()->SaveScore(levelName, playerName, score);

        SetVisible(false);
        mlg::SceneManager::ExitGame();
    });

    SetVisible(false);
}

void FinishScreen::SetScore(int score, const std::string& levelName) {
    auto sharedScoreLabel = scoreLabel.lock();
    sharedScoreLabel->SetText(fmt::format("Score: ${}", score));

    UpdateScoreBoard(score, levelName);
}

void FinishScreen::SetVisible(bool visible) {
    background.lock()->SetVisible(visible);
    headerText.lock()->SetVisible(visible);
    leftBackground.lock()->SetVisible(visible);
    exitButton.lock()->SetVisible(visible);
    exitButton.lock()->SetActive(visible);
    scoreLabel.lock()->SetVisible(visible);
    rightBackground.lock()->SetVisible(visible);
    rightHeaderText.lock()->SetVisible(visible);
    scoreBoard.lock()->SetVisible(visible);

    for (auto& characterSelector : characterSelectors) {
        characterSelector.lock()->SetVisible(visible);
        characterSelector.lock()->SetActive(visible);
    }

    characterSelectors[0].lock()->GrabFocus();
}

void FinishScreen::FinishScreen::CreateBackground() {
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

    CreateCharacterSelectors();
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

    scoreBoard = AddComponent<mlg::Label>(
            "scoreboard",
            mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));

    auto sharedScoreboard = scoreBoard.lock();
    sharedScoreboard->SetSize(28);

    sharedScoreboard->SetPosition(
            MLG_POS_CENTER + glm::vec2{
                                     PANEL_SIZE.x * 0.24,
                                     PANEL_SIZE.y * 0.16});
    sharedScoreboard->SetAnchor(MLG_ANCHOR_CENTER);
    sharedScoreboard->SetVerticalAlignment(mlg::Label::VerticalAlignment::Up);
    sharedScoreboard->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
}

void FinishScreen::CreateCharacterSelectors() {
    float offset = -1.5;
    for (auto& selector : characterSelectors) {
        selector = AddComponent<mlg::CharacterSelector>(
                "character_selector",
                mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_MATERIAL),
                mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_FOCUSED_MATERIAL),
                mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));
        auto sharedSelector = selector.lock();
        sharedSelector->SetSize(SELECTOR_SIZE);
        sharedSelector->SetAnchor(MLG_ANCHOR_CENTER);
        sharedSelector->SetPosition(
                MLG_POS_CENTER + glm::vec2{
                                         -PANEL_SIZE.x * 0.24 + SELECTOR_SIZE.x * 1.5f * offset,
                                         -PANEL_SIZE.y * 0.05});

        offset++;
    }

    int i = 0;
    for (auto& selector : characterSelectors) {
        auto sharedSelector = selector.lock();
        sharedSelector->SetCharacter('A');

        selector.lock()->next.right = characterSelectors[(i + 1) % characterSelectors.size()];
        selector.lock()->next.left = characterSelectors[(i - 1) % characterSelectors.size()];
        i++;
    }

    characterSelectors[0].lock()->next.left = exitButton;
    characterSelectors[characterSelectors.size() - 1].lock()->next.right = exitButton;

    exitButton.lock()->next.right = characterSelectors[0];
    exitButton.lock()->next.left = characterSelectors[characterSelectors.size() - 1];

    exitButton.lock()->next.bottom = characterSelectors[0];
    exitButton.lock()->next.top = characterSelectors[0];
}

void FinishScreen::UpdateScoreBoard(int currentScore, const std::string& levelName) {
    auto scores = ScoreManager::GetScoreBoard(levelName);

    scores.insert(ScoreManager::ScoreBoardEntry{"> NOW", currentScore});

    auto scoreCMP = [](const auto& a, const auto& b) {
        return a.score < b.score;
    };

    std::string scoreBoardText;
    for (auto& score : scores) {
        std::string scoreEntry = fmt::format("{:5} {:05d}", score.playerName, score.score);
        scoreBoardText += fmt::format("{}\n", scoreEntry);
    }

    scoreBoard.lock()->SetText(scoreBoardText);
}

std::string FinishScreen::GetUserName() {
    std::string name;
    for (auto& selector : characterSelectors) {
        name += selector.lock()->GetCharacter();
    }

    return name;
}
