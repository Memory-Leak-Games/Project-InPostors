#include "UI/PauseMenu.h"
#include "Core/SceneManager/Scene.h"
#include "Scenes/MenuScene.h"
#include "UI/Builders/ButtonBuilder.h"
#include "UI/Components/Button.h"

#include "UI/Components/Button.h"
#include "UI/Components/Containers/CanvasPanel.h"
#include "UI/Components/Containers/Container.h"
#include "UI/Components/Containers/HorizontalBox.h"
#include "UI/Components/Containers/VerticalBox.h"
#include "UI/Components/DynamicImage.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"

#include "Core/SceneManager/SceneManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Assets/FontAsset.h"

#include "UI/Components/UIComponent.h"
#include "UI/UIStyle.h"

PauseMenu::PauseMenu(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {
}

std::shared_ptr<PauseMenu> PauseMenu::Create(
        uint64_t id,
        const std::string& name,
        bool isStatic,
        mlg::Transform* parent) {
    std::shared_ptr<PauseMenu> pauseMenu = std::shared_ptr<PauseMenu>(
            new PauseMenu(id, name, isStatic, parent));


    std::weak_ptr<mlg::Image> background = pauseMenu->AddComponent<mlg::Image>(
            "Background",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL));
    background.lock()->SetSize(3.f * SCREEN_SIZE);

    std::weak_ptr<mlg::Image> subBackground = pauseMenu->AddComponent<mlg::Image>(
            "SubBackground",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL));
    subBackground.lock()->SetSize(BACKGROUND_SIZE);

    pauseMenu->inputMap = pauseMenu->AddComponent<mlg::DynamicImage>("InputMap");
    pauseMenu->inputMap.lock()->SetTexture("res/textures/tutorial/gamepad_texture.png");
    pauseMenu->inputMap.lock()->SetSize({768, 432.f});
    pauseMenu->inputMap.lock()->SetRelativePosition(MLG_POS_CENTER + glm::vec2{175, 0});
    pauseMenu->inputMap.lock()->SetAnchor(MLG_ANCHOR_LEFT);

    std::weak_ptr<mlg::VerticalBox> vbox =
            pauseMenu->AddComponent<mlg::VerticalBox>("VBox");

    pauseMenu->AddResumeButton(vbox.lock().get());
    pauseMenu->AddExitButton(vbox.lock().get());

    pauseMenu->menuPanel = pauseMenu->AddComponent<mlg::CanvasPanel>("MenuPanel");
    pauseMenu->menuPanel.lock()->SetSize(BACKGROUND_SIZE);
    pauseMenu->menuPanel.lock()->SetRelativePosition(BUTTON_BASE_POSITION);
    pauseMenu->menuPanel.lock()->AddChild(background);
    pauseMenu->menuPanel.lock()->AddChild(subBackground);

    pauseMenu->menuPanel.lock()->AddChild(vbox);
    pauseMenu->SetVisible(false);

    return pauseMenu;
}

void PauseMenu::AddResumeButton(mlg::Container* container) {
    mlg::ButtonBuilder buttonBuilder;
    auto sharedResumeButton =
            buttonBuilder.SetName("ResumeButton")
                    .SetSize(BUTTON_SIZE)
                    .SetText("Resume")
                    .BuildButton(this)
                    .lock();

    container->AddChild(sharedResumeButton);

    sharedResumeButton->OnClick.append([this]() {
        this->SetVisible(false);
        mlg::Time::PauseGame(false);
    });
}

void PauseMenu::AddExitButton(mlg::Container* container) {
    mlg::ButtonBuilder buttonBuilder;
    auto sharedExitButton =
            buttonBuilder.SetName("ExitButton")
                    .SetSize(BUTTON_SIZE)
                    .SetText("Exit")
                    .BuildButton(this)
                    .lock();

    container->AddChild(sharedExitButton);

    sharedExitButton->OnClick.append([this]() {
        auto menuScene = std::make_unique<MenuScene>();
        mlg::SceneManager::SetNextScene(std::move(menuScene));
    });
}

void PauseMenu::SetVisible(bool visible) {
    menuPanel.lock()->SetVisible(visible);
    inputMap.lock()->SetVisible(visible);

    if (visible == true)
        menuPanel.lock()->GrabFocus();
}