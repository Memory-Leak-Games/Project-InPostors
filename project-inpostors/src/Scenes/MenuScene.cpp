#include "Scenes/MenuScene.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/SceneManager/SceneManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"
#include "Macros.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "SceneGraph/SceneGraph.h"

#include "Scenes/TestScene.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Builders/ButtonBuilder.h"
#include "UI/Components/Button.h"
#include "UI/Components/OptionSelector.h"
#include "UI/Components/Containers/CanvasPanel.h"
#include "UI/Components/Containers/Container.h"
#include "UI/Components/Containers/VerticalBox.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/Spacer.h"
#include "UI/Components/UIComponent.h"

#include "UI/UIStyle.h"

MenuScene::~MenuScene() = default;

void MenuScene::Load() {
    auto entity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
            "MenuBackground", false, mlg::SceneGraph::GetRoot());

    // TODO: Remove this
    auto catMat =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/cat_ui_material.json");
    auto catIm = entity.lock()->AddComponent<mlg::Image>("Water", catMat);
    catIm.lock()->SetSize(MLG_FULL_SIZE);
    catIm.lock()->SetAnchor(MLG_ANCHOR_CENTER);
    catIm.lock()->SetRelativePosition(MLG_POS_CENTER);


    InitializeMainMenu();
    mainMenuContainer.lock()->GrabFocus();

    InitializeCredits();
    creditsContainer.lock()->SetVisible(false);

    InitializeSettings();
    settingsContainer.lock()->SetVisible(false);
}

void MenuScene::InitializeMainMenu() {
    auto entity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
            "MainMenu", false, mlg::SceneGraph::GetRoot());

    mainMenuContainer = entity.lock()->AddComponent<mlg::CanvasPanel>("MainMenu");
    mainMenuContainer.lock()->SetAnchor(MLG_ANCHOR_LEFT);
    mainMenuContainer.lock()->SetRelativePosition(
            {BUTTON_BASE_POSITION.x, MLG_POS_CENTER.y});
    mainMenuContainer.lock()->SetSize(PANEL_SIZE);

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL);
    auto menuBackground =
            entity.lock()->AddComponent<mlg::Image>("MenuBackground", material);
    menuBackground.lock()->SetSize(BACKGROUND_SIZE);
    menuBackground.lock()->SetAnchor(MLG_ANCHOR_LEFT);
    mainMenuContainer.lock()->AddChild(menuBackground);

    auto vbox =
            entity.lock()->AddComponent<mlg::VerticalBox>("VBox");
    mainMenuContainer.lock()->AddChild(vbox);

    auto logoMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    "res/materials/ui/logo_material.json");

    auto gameLogo = entity.lock()->AddComponent<mlg::Image>(
            "GameLogo",
            logoMaterial);

    gameLogo.lock()->SetSize(glm::vec2(BUTTON_SIZE.x, BUTTON_SIZE.x));
    gameLogo.lock()->SetPadding(10.f);
    vbox.lock()->AddChild(gameLogo);

    auto spacer = entity.lock()->AddComponent<mlg::Spacer>("Spacer");
    spacer.lock()->SetSize(glm::vec2(3.f * BUTTON_SIZE.y));
    vbox.lock()->AddChild(spacer);

    mlg::ButtonBuilder buttonBuilder;
    buttonBuilder = buttonBuilder
                            .SetSize(BUTTON_SIZE)
                            .SetAnchor(MLG_ANCHOR_LEFT)
                            .SetPadding(10.f);

    auto playButton =
            buttonBuilder.SetName("PlayButton")
                    .SetText("Play")
                    .BuildButton(entity.lock().get());
    BindToOnPlay(*playButton.lock());
    vbox.lock()->AddChild(playButton);

    auto settingsButton =
            buttonBuilder.SetName("SettingsButton")
                    .SetText("Settings")
                    .BuildButton(entity.lock().get());
    vbox.lock()->AddChild(settingsButton);
    BindToOnSettings(*settingsButton.lock());

    auto creditsButton =
            buttonBuilder.SetName("CreditsButton")
                    .SetText("Credits")
                    .BuildButton(entity.lock().get());
    BindToOnCredits(*creditsButton.lock());
    vbox.lock()->AddChild(creditsButton);

    auto exitButton =
            buttonBuilder.SetName("ExitButton")
                    .SetText("Exit")
                    .BuildButton(entity.lock().get());
    vbox.lock()->AddChild(exitButton);
    BindToOnExit(*exitButton.lock());
}

void MenuScene::BindToOnExit(mlg::Button& button) {
    button.OnClick.append(
            []() {
                mlg::SceneManager::ExitGame();
            });
}

// TODO: Update me
void MenuScene::BindToOnPlay(mlg::Button& button) {
    button.OnClick.append(
            []() {
                auto testScene = std::make_unique<TestScene>();
                mlg::SceneManager::SetNextScene(std::move(testScene));
            });
}

void MenuScene::BindToOnCredits(mlg::Button& button) {
    button.OnClick.append(
            [this]() {
                mainMenuContainer.lock()->SetVisible(false);
                creditsContainer.lock()->SetVisible(true);
                creditsContainer.lock()->GrabFocus();
            });
}

void MenuScene::BindToOnSettings(mlg::Button& button) {
    button.OnClick.append(
            [this]() {
                mainMenuContainer.lock()->SetVisible(false);
                settingsContainer.lock()->SetVisible(true);
                settingsContainer.lock()->GrabFocus();
            });
}

void MenuScene::InitializeCredits() {
    auto entity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
            "Credits", false, mlg::SceneGraph::GetRoot());

    creditsContainer = entity.lock()->AddComponent<mlg::CanvasPanel>("Credits");

    creditsContainer.lock()->SetSize(PANEL_SIZE);
    creditsContainer.lock()->SetAnchor(MLG_ANCHOR_CENTER);
    creditsContainer.lock()->SetRelativePosition(MLG_POS_CENTER);

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL);
    auto background =
            entity.lock()->AddComponent<mlg::Image>("MenuBackground", material);
    background.lock()->SetSize(PANEL_SIZE);

    creditsContainer.lock()->AddChild(background);

    auto vbox = entity.lock()->AddComponent<mlg::VerticalBox>("VBox");
    creditsContainer.lock()->AddChild(vbox);

    auto creditsMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    "res/materials/ui/credits_material.json");

    auto creditsImage =
            entity.lock()->AddComponent<mlg::Image>(
                    "CreditsImage", creditsMaterial);
    creditsImage.lock()->SetSize(glm::vec2(400.f, 400.f));
    creditsImage.lock()->SetPadding(10.f);

    vbox.lock()->AddChild(creditsImage);

    mlg::ButtonBuilder buttonBuilder;
    auto backButton = buttonBuilder
                              .SetSize(BUTTON_SIZE)
                              .SetPadding(10.f)
                              .SetName("BackButton")
                              .SetText("Back")
                              .BuildButton(entity.lock().get());
    BindToBackToMainMenu(*backButton.lock(), *creditsContainer.lock());
    vbox.lock()->AddChild(backButton);
}

void MenuScene::BindToBackToMainMenu(mlg::Button& button, mlg::Container& container) {
    button.OnClick.append(
            [this, &container]() {
                container.SetVisible(false);
                mainMenuContainer.lock()->SetVisible(true);
                mainMenuContainer.lock()->GrabFocus();
            });
}

void MenuScene::InitializeSettings() {
    auto entity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
            "SettingsMenu", false, mlg::SceneGraph::GetRoot());

    settingsContainer = entity.lock()->AddComponent<mlg::CanvasPanel>("SettingsMenu");
    settingsContainer.lock()->SetAnchor(MLG_ANCHOR_LEFT);
    settingsContainer.lock()->SetRelativePosition(
            {BUTTON_BASE_POSITION.x, MLG_POS_CENTER.y});
    settingsContainer.lock()->SetSize(PANEL_SIZE);

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL);
    auto background =
            entity.lock()->AddComponent<mlg::Image>("MenuBackground", material);
    background.lock()->SetSize(BACKGROUND_SIZE);
    background.lock()->SetAnchor(MLG_ANCHOR_LEFT);
    settingsContainer.lock()->AddChild(background);

    auto vbox =
            entity.lock()->AddComponent<mlg::VerticalBox>("VBox");
    settingsContainer.lock()->AddChild(vbox);

    mlg::ButtonBuilder buttonBuilder;
    buttonBuilder = buttonBuilder
                            .SetSize(BUTTON_SIZE)
                            .SetPadding(10.f);

    auto windowTypeSwitcher =
            buttonBuilder
                    .SetName("WindowTypeSwitcher")
                    .SetText("Window Type")
                    .BuildOptionSwitcher(entity.lock().get());
    auto windowTypes = std::array{
            "Windowed",
            "Borderless",
            "Fullscreen"
    };
    
    windowTypeSwitcher.lock()->AddOptions(windowTypes.begin(), windowTypes.end());
    vbox.lock()->AddChild(windowTypeSwitcher);

    auto separator = entity.lock()->AddComponent<mlg::Spacer>("Spacer");
    separator.lock()->SetSize(BUTTON_SIZE);
    vbox.lock()->AddChild(separator);

    auto applyButton =
            buttonBuilder
                    .SetName("ApplyButton")
                    .SetText("Apply")
                    .BuildButton(entity.lock().get());
    BindToOnApply(*applyButton.lock());
    vbox.lock()->AddChild(applyButton);

    


    auto backButton = buttonBuilder
                              .SetName("BackButton")
                              .SetText("Back")
                              .BuildButton(entity.lock().get());
    BindToBackToMainMenu(*backButton.lock(), *settingsContainer.lock());
    vbox.lock()->AddChild(backButton);
}

void MenuScene::BindToOnApply(mlg::Button& button) {
    button.OnClick.append(
            [this]() {
                MLG_UNIMPLEMENTED_SOFT;
            });
}
