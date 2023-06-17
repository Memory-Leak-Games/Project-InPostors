#include "Scenes/MenuScene.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/SceneManager/SceneManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "SceneGraph/SceneGraph.h"

#include "UI/Assets/FontAsset.h"
#include "UI/Builders/ButtonBuilder.h"
#include "UI/Components/Button.h"
#include "UI/Components/Containers/CanvasPanel.h"
#include "UI/Components/Containers/VerticalBox.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/Spacer.h"
#include "UI/Components/UIComponent.h"

#include "UI/UIStyle.h"
#include <glm/fwd.hpp>

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
    //     mainMenuContainer.lock()->SetVisible(false);

    InitializeCredits();
    creditsContainer.lock()->SetVisible(false);
}

void MenuScene::InitializeMainMenu() {
    auto entity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
            "MainMenu", false, mlg::SceneGraph::GetRoot());

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL);
    menuBackground = entity.lock()->AddComponent<mlg::Image>("MenuBackground", material);
    menuBackground.lock()->SetSize(BACKGROUND_SIZE);
    menuBackground.lock()->SetAnchor(MLG_ANCHOR_LEFT);
    menuBackground.lock()->SetRelativePosition(BUTTON_BASE_POSITION);

    // TODO: Remove this
    menuBackground.lock()->SetVisible(false);

    mainMenuContainer =
            entity.lock()->AddComponent<mlg::VerticalBox>("VBox");

    mainMenuContainer.lock()->SetAnchor(MLG_ANCHOR_LEFT);
    mainMenuContainer.lock()->SetRelativePosition(
            glm::vec2{BUTTON_BASE_POSITION.x, MLG_POS_CENTER.y});

    auto logoMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    "res/materials/ui/logo_material.json");

    auto gameLogo = entity.lock()->AddComponent<mlg::Image>(
            "GameLogo",
            logoMaterial);

    gameLogo.lock()->SetSize(glm::vec2(BUTTON_SIZE.x, BUTTON_SIZE.x));
    gameLogo.lock()->SetPadding(10.f);
    mainMenuContainer.lock()->AddChild(gameLogo);

    auto spacer = entity.lock()->AddComponent<mlg::Spacer>("Spacer");
    spacer.lock()->SetSize(glm::vec2(3.f * BUTTON_SIZE.y));
    mainMenuContainer.lock()->AddChild(spacer);

    mlg::ButtonBuilder buttonBuilder;
    buttonBuilder = buttonBuilder
                            .SetSize(BUTTON_SIZE)
                            .SetAnchor(MLG_ANCHOR_LEFT)
                            .SetPadding(10.f);

    auto playButton =
            buttonBuilder.SetName("PlayButton")
                    .SetText("Play")
                    .Build(entity.lock().get());
    mainMenuContainer.lock()->AddChild(playButton);

    auto settingsButton =
            buttonBuilder.SetName("SettingsButton")
                    .SetText("Settings")
                    .Build(entity.lock().get());
    mainMenuContainer.lock()->AddChild(settingsButton);

    auto creditsButton =
            buttonBuilder.SetName("CreditsButton")
                    .SetText("Credits")
                    .Build(entity.lock().get());
    mainMenuContainer.lock()->AddChild(creditsButton);

    auto exitButton =
            buttonBuilder.SetName("ExitButton")
                    .SetText("Exit")
                    .Build(entity.lock().get());
    mainMenuContainer.lock()->AddChild(exitButton);
    BindToOnExit(exitButton.lock());

    mainMenuContainer.lock()->GrabFocus();
}

void MenuScene::BindToOnExit(const std::shared_ptr<mlg::Button>& button) {
    button->OnClick.append(
            []() {
                mlg::SceneManager::ExitGame();
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
                              .Build(entity.lock().get());

    vbox.lock()->AddChild(backButton);
}
