#include "Scenes/MenuScene.h"

#include "Audio/Audio.h"
#include "Audio/AudioAPI.h"
#include "Core/AssetManager/AssetManager.h"
#include "Core/SceneManager/SceneManager.h"
#include "Core/Settings/SettingsManager.h"
#include "Core/Window.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"
#include "Macros.h"
#include "Managers/AudioManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "SceneGraph/SceneGraph.h"

#include "Scenes/TestScene.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Builders/ButtonBuilder.h"
#include "UI/Builders/LabelBuilder.h"
#include "UI/Components/Button.h"
#include "UI/Components/Containers/CanvasPanel.h"
#include "UI/Components/Containers/Container.h"
#include "UI/Components/Containers/HorizontalBox.h"
#include "UI/Components/Containers/VerticalBox.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/OptionSelector.h"
#include "UI/Components/Spacer.h"
#include "UI/Components/UIComponent.h"

#include "UI/UIStyle.h"
#include <fstream>
#include <glm/fwd.hpp>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>
#include <string>

#define LEVELS_FILE "res/levels/levels.json"

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

    InitializeLevelSelector();
    levelSelector.lock()->SetVisible(false);
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
            [this]() {
                mainMenuContainer.lock()->SetVisible(false);
                levelSelector.lock()->SetVisible(true);
                levelSelector.lock()->GrabFocus();
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

    mlg::LabelBuilder labelBuilder;
    labelBuilder = labelBuilder
                           .SetPadding(50.f)
                           .SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center)
                           .SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);

    // WINDOW SETTINGS
    auto windowLabel = labelBuilder
                               .SetName("WindowLabel")
                               .SetText("Window Settings")
                               .BuildLabel(entity.lock().get());
    vbox.lock()->AddChild(windowLabel);

    mlg::ButtonBuilder buttonBuilder;
    buttonBuilder = buttonBuilder
                            .SetSize(BUTTON_SIZE)
                            .SetPadding(10.f);

    auto windowSettings =
            buttonBuilder
                    .SetName("WindowTypeSwitcher")
                    .SetText("Window Type")
                    .BuildSelector(entity.lock().get());
    auto windowTypes = std::array{
            "Windowed", "Borderless", "Fullscreen"};

    windowSettings.lock()->AddOptions(windowTypes.begin(), windowTypes.end());
    vbox.lock()->AddChild(windowSettings);

    // GRAPHICS SETTINGS
    auto graphicsLabel = labelBuilder
                                 .SetName("GraphicsLabel")
                                 .SetText("Graphics Settings")
                                 .BuildLabel(entity.lock().get());
    vbox.lock()->AddChild(graphicsLabel);

    auto graphicsSettings =
            buttonBuilder
                    .SetName("WindowTypeSwitcher")
                    .SetText("Window Type")
                    .BuildSelector(entity.lock().get());
    auto settingsTypes = std::array{
            "Low", "Medium", "InPostible"};

    graphicsSettings.lock()->AddOptions(settingsTypes.begin(), settingsTypes.end());
    vbox.lock()->AddChild(graphicsSettings);

    // Volume
    auto volumeLabel = labelBuilder
                               .SetName("VolumeLabel")
                               .SetText("Volume")
                               .BuildLabel(entity.lock().get());
    vbox.lock()->AddChild(volumeLabel);

    auto volumeSetings = buttonBuilder
                                 .SetName("VolumeSettings")
                                 .SetText("Volume")
                                 .BuildSelector(entity.lock().get());
    auto volumeTypes = std::array{
            "0%", "25%", "50%", "75%", "100%"};
    volumeSetings.lock()->AddOptions(volumeTypes.begin(), volumeTypes.end());
    vbox.lock()->AddChild(volumeSetings);

    LoadSettings(
            *windowSettings.lock(),
            *graphicsSettings.lock(),
            *volumeSetings.lock());

    // MENU
    auto separator = entity.lock()->AddComponent<mlg::Spacer>("Spacer");
    separator.lock()->SetSize(BUTTON_SIZE);
    vbox.lock()->AddChild(separator);

    auto applyButton =
            buttonBuilder
                    .SetName("ApplyButton")
                    .SetText("Apply")
                    .BuildButton(entity.lock().get());
    BindToOnApply(
            *applyButton.lock(), *windowSettings.lock(),
            *graphicsSettings.lock(), *volumeSetings.lock());
    vbox.lock()->AddChild(applyButton);


    auto backButton = buttonBuilder
                              .SetName("BackButton")
                              .SetText("Back")
                              .BuildButton(entity.lock().get());
    BindToBackToMainMenu(*backButton.lock(), *settingsContainer.lock());
    vbox.lock()->AddChild(backButton);
}

void MenuScene::LoadSettings(mlg::OptionSelector& windowMode,
                             mlg::OptionSelector& graphicsMode,
                             mlg::OptionSelector& volume) {
    auto windowTypeString =
            mlg::SettingsManager::Get<std::string>(
                    mlg::SettingsType::Video, "WindowType");
    auto windowType =
            magic_enum::enum_cast<mlg::WindowType>(windowTypeString).value();
    int windowTypeIndex = magic_enum::enum_integer(windowType);
    windowMode.SetOption(windowTypeIndex);

    int graphicsQuality = mlg::SettingsManager::Get<int>(
            mlg::SettingsType::Video, "GraphicsQuality");
    graphicsMode.SetOption(graphicsQuality);

    float volumeValue = mlg::SettingsManager::Get<float>(
            mlg::SettingsType::Audio, "volume");
    int volumeIndex = std::ceil(volumeValue / 0.25f);
    volume.SetOption(volumeIndex);
}

void MenuScene::BindToOnApply(mlg::Button& button,
                              mlg::OptionSelector& windowMode,
                              mlg::OptionSelector& graphicsMode,
                              mlg::OptionSelector& volume) {

    button.OnClick.append(
            [this, &windowMode, &graphicsMode, &volume]() {
                SetWindowSettings(windowMode);
                SetGraphicsSettings(graphicsMode);
                SetVolumeSettings(volume);

                mlg::SettingsManager::Save();

                settingsContainer.lock()->SetVisible(false);
                mainMenuContainer.lock()->SetVisible(true);
                mainMenuContainer.lock()->GrabFocus();
            });
}

void MenuScene::SetWindowSettings(mlg::OptionSelector& windowMode) {
    auto windowType = magic_enum::enum_cast<mlg::WindowType>(
            windowMode.GetOption());
    mlg::Window::Get()->SetWindowType(windowType.value());

    SPDLOG_DEBUG("Window: {}", magic_enum::enum_name(windowType.value()));

    mlg::SettingsManager::Set(
            mlg::SettingsType::Video,
            "WindowType",
            magic_enum::enum_name(windowType.value()));
}

void MenuScene::SetGraphicsSettings(mlg::OptionSelector& graphicsMode) {
    int option = graphicsMode.GetOption();

    SPDLOG_DEBUG("Graphics: {}", graphicsMode.GetOption());

    if (option == 2) {// Max
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "FXAA",
                true);
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "SSAO",
                true);
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "SSAOSamples",
                32);
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "ShadowMapResolution",
                4096);
    } else if (option == 1) {
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "FXAA",
                true);
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "SSAO",
                true);
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "SSAOSamples",
                16);
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "ShadowMapResolution",
                2048);
    } else if (option == 0) {
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "FXAA",
                false);
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "SSAO",
                false);
        mlg::SettingsManager::Set(
                mlg::SettingsType::Video,
                "ShadowMapResolution",
                512);
    }

    mlg::SettingsManager::Set(
            mlg::SettingsType::Video,
            "GraphicsQuality",
            option);
}

void MenuScene::SetVolumeSettings(mlg::OptionSelector& volume) {
    float volumeAmount = volume.GetOption() * 0.25f;

    SPDLOG_DEBUG("Volume: {}%", volumeAmount);
    mlg::AudioAPI::Get()->SetVolume(volumeAmount);
    mlg::SettingsManager::Set(
            mlg::SettingsType::Audio,
            "volume",
            volumeAmount);
}

void MenuScene::InitializeLevelSelector() {
    auto entity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
            "Level Selector", false, mlg::SceneGraph::GetRoot());

    levelSelector = entity.lock()->AddComponent<mlg::CanvasPanel>("LevelSelector");
    levelSelector.lock()->SetSize(PANEL_SIZE);
    levelSelector.lock()->SetAnchor(MLG_ANCHOR_CENTER);
    levelSelector.lock()->SetRelativePosition(MLG_POS_CENTER);

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL);
    auto background =
            entity.lock()->AddComponent<mlg::Image>("MenuBackground", material);
    background.lock()->SetSize(PANEL_SIZE);

    levelSelector.lock()->AddChild(background);

    auto hbox = entity.lock()->AddComponent<mlg::HorizontalBox>("HBox");
    levelSelector.lock()->AddChild(hbox);

    std::ifstream fileStream(LEVELS_FILE);
    nlohmann::json levelsJson = nlohmann::json::parse(fileStream);

    for (const auto& level : levelsJson) {
        auto levelPanel = LoadLevelPanel(level, entity.lock().get());
        hbox.lock()->AddChild(levelPanel);
    }

    mlg::ButtonBuilder buttonBuilder;
    auto backButton = buttonBuilder
                              .SetSize(BUTTON_SIZE)
                              .SetAnchor(MLG_ANCHOR_CENTER)
                              .SetPadding(10.f)
                              .SetName("BackButton")
                              .SetText("Back")
                              .BuildButton(entity.lock().get());
    backButton.lock()->SetRelativePosition(-glm::vec2(0.f, 200.f));
    BindToBackToMainMenu(*backButton.lock(), *levelSelector.lock());
    levelSelector.lock()->AddChild(backButton);
}

std::weak_ptr<mlg::Container> MenuScene::LoadLevelPanel(
        const nlohmann::json& levelsJson, mlg::Entity* entity) {
    auto levelPanel = entity->AddComponent<mlg::CanvasPanel>("LevelPanel");
    levelPanel.lock()->SetSize(glm::vec2{256.f, 256.f});
    levelPanel.lock()->SetPadding(20.f);

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(WHITE_MATERIAL);
    auto background =
            entity->AddComponent<mlg::Image>("MenuBackground", material);
    background.lock()->SetSize(glm::vec2{256.f, 256.f});

    levelPanel.lock()->AddChild(background);

    auto vbox = entity->AddComponent<mlg::VerticalBox>("VBox");
    levelSelector.lock()->AddChild(vbox);
    vbox.lock()->SetPadding(30.f);

    auto levelImageMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(levelsJson["material"]);

    auto levelImage =
            entity->AddComponent<mlg::Image>("levelImage", levelImageMaterial);
    levelImage.lock()->SetSize(glm::vec2(128.f, 128.f));
    levelImage.lock()->SetPadding(20.f);
    vbox.lock()->AddChild(levelImage);

    mlg::LabelBuilder labelBuilder;
    auto levelLabel = labelBuilder
                              .SetPadding(50.f)
                              .SetTextColor(glm::vec3(0.f))
                              .SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center)
                              .SetVerticalAlignment(mlg::Label::VerticalAlignment::Center)
                              .SetAnchor(MLG_ANCHOR_CENTER)
                              .SetSize(32)
                              .SetText(levelsJson["name"])
                              .SetName("LevelLabel")
                              .BuildLabel(entity);
    vbox.lock()->AddChild(levelLabel);
    levelPanel.lock()->AddChild(vbox);

    return levelPanel;
}
