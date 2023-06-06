#include "UI/PauseMenu.h"
#include "UI/Components/Button.h"

#include "UI/Components/Button.h"
#include "UI/Components/Image.h"

#include "Rendering/Assets/MaterialAsset.h"

#define BUTTON_MATERIAL "res/materials/ui/buttons/button_material.json"
#define BUTTON_FOCUSED_MATERIAL "res/materials/ui/buttons/button_focused_material.json"
#define BACKGROUND_MATERIAL "res/materials/ui/semi_transparent_background_material.json"

#define BUTTON_SIZE glm::vec2{ 250, 50 }
#define BUTTON_BASE_POSITION glm::vec2{ 200, 150 }

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
    pauseMenu->background.lock()->SetSize(glm::vec2(BUTTON_SIZE.x * 1.25, 4 * 720));
    pauseMenu->background.lock()->SetPosition(BUTTON_BASE_POSITION);

    pauseMenu->AddResumeButton();
    pauseMenu->AddExitButton();

    pauseMenu->resumeButton.lock()->next.bottom = pauseMenu->exitButton;
    pauseMenu->exitButton.lock()->next.top = pauseMenu->resumeButton;

    // pauseMenu->SetVisible(true);

    return pauseMenu;
}

void PauseMenu::AddResumeButton() {
    resumeButton = this->AddComponent<mlg::Button>(
            "ResumeButton",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_MATERIAL),
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_FOCUSED_MATERIAL));

    auto sharedResumeButton = resumeButton.lock();
    sharedResumeButton->SetSize(BUTTON_SIZE);
    sharedResumeButton->SetPosition(BUTTON_BASE_POSITION + glm::vec2(0.f, 30.f));
    sharedResumeButton->GrabFocus();
}

void PauseMenu::AddExitButton() {
    exitButton = this->AddComponent<mlg::Button>(
            "ExitButton",
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_MATERIAL),
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_FOCUSED_MATERIAL));

    auto sharedExitButton = exitButton.lock();
    sharedExitButton->SetSize(BUTTON_SIZE);
    sharedExitButton->SetPosition(BUTTON_BASE_POSITION + glm::vec2(0.f, -30.f));
}

void PauseMenu::SetVisible(bool visible) {
    background.lock()->SetVisible(visible);

    resumeButton.lock()->SetActive(visible);
    resumeButton.lock()->SetVisible(visible);

    exitButton.lock()->SetActive(visible);
    exitButton.lock()->SetVisible(visible);
}