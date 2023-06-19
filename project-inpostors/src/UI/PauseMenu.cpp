#include "UI/PauseMenu.h"
#include "Core/SceneManager/Scene.h"
#include "Scenes/MenuScene.h"
#include "UI/Components/Button.h"

#include "UI/Components/Button.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Assets/FontAsset.h"
#include "Core/SceneManager/SceneManager.h"

#include "UI/UIStyle.h"
#include <memory>

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
    auto pauseMenu = std::shared_ptr<PauseMenu>(
            new PauseMenu(id, name, isStatic, parent));

    pauseMenu->background = pauseMenu->AddComponent<mlg::Image>(
            "Background",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BACKGROUND_MATERIAL));
    pauseMenu->background.lock()->SetSize(BACKGROUND_SIZE);
    pauseMenu->background.lock()->SetRelativePosition(BUTTON_BASE_POSITION);

    pauseMenu->AddResumeButton();
    pauseMenu->AddExitButton();

    pauseMenu->resumeButton.lock()->next.bottom = pauseMenu->exitButton;
    pauseMenu->exitButton.lock()->next.top = pauseMenu->resumeButton;

    pauseMenu->SetVisible(false);

    return pauseMenu;
}

void PauseMenu::AddResumeButton() {
    resumeButton = this->AddComponent<mlg::Button>(
            "ResumeButton",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_MATERIAL),
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_FOCUSED_MATERIAL),
            mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));

    auto sharedResumeButton = resumeButton.lock();
    sharedResumeButton->SetSize(BUTTON_SIZE);
    sharedResumeButton->SetRelativePosition(BUTTON_BASE_POSITION + glm::vec2(0.f, 30.f));
    sharedResumeButton->GetLabel().lock()->SetTextColor(glm::vec3(0.f));
    sharedResumeButton->GetLabel().lock()->SetText("Resume");

    sharedResumeButton->OnClick.append([this]() {
        this->SetVisible(false);
        mlg::Time::PauseGame(false);
    });
}

void PauseMenu::AddExitButton() {
    exitButton = this->AddComponent<mlg::Button>(
            "ExitButton",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_MATERIAL),
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_FOCUSED_MATERIAL),
            mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));

    auto sharedExitButton = exitButton.lock();
    sharedExitButton->SetSize(BUTTON_SIZE);
    sharedExitButton->SetRelativePosition(BUTTON_BASE_POSITION + glm::vec2(0.f, -30.f));
    sharedExitButton->GetLabel().lock()->SetTextColor(glm::vec3(0.f));
    sharedExitButton->GetLabel().lock()->SetText("Exit");

    sharedExitButton->OnClick.append([this]() {
        auto menuScene = std::make_unique<MenuScene>();
        mlg::SceneManager::SetNextScene(std::move(menuScene));
    });
}

void PauseMenu::SetVisible(bool visible) {
    background.lock()->SetVisible(visible);

    resumeButton.lock()->SetActive(visible);
    resumeButton.lock()->SetVisible(visible);

    exitButton.lock()->SetActive(visible);
    exitButton.lock()->SetVisible(visible);

    resumeButton.lock()->GrabFocus();
}