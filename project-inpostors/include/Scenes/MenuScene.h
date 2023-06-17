#pragma once

#include "Core/SceneManager/Scene.h"

namespace mlg {
    class Container;
    class CanvasPanel;
    class Image;
    class Button;
    class Label;
    class OptionSelector;
    class Entity;
}// namespace mlg

class MenuScene : public mlg::Scene {
private:
    std::weak_ptr<mlg::CanvasPanel> mainMenuContainer;
    std::weak_ptr<mlg::CanvasPanel> creditsContainer;
    std::weak_ptr<mlg::CanvasPanel> settingsContainer;

    std::weak_ptr<mlg::CanvasPanel> levelSelector;

public:
    explicit MenuScene() = default;
    ~MenuScene() override;

    void Load() override;

private:
    void InitializeMainMenu();
    void BindToOnExit(mlg::Button& button);
    void BindToOnPlay(mlg::Button& button);
    void BindToOnCredits(mlg::Button& button);
    void BindToOnSettings(mlg::Button& button);

    void InitializeCredits();

    void InitializeSettings();
    void BindToOnApply(mlg::Button& button,
                       mlg::OptionSelector& windowMode,
                       mlg::OptionSelector& graphicsMode,
                       mlg::OptionSelector& volume);
    void LoadSettings(mlg::OptionSelector& windowMode,
                      mlg::OptionSelector& graphicsMode,
                      mlg::OptionSelector& volume);

    void BindToBackToMainMenu(mlg::Button& button, mlg::Container& container);

    void SetWindowSettings(mlg::OptionSelector& windowMode);
    void SetGraphicsSettings(mlg::OptionSelector& graphicsMode);
    void SetVolumeSettings(mlg::OptionSelector& volume);

    void InitializeLevelSelector();
    std::weak_ptr<mlg::Button> LoadLevelButton(
        const nlohmann::json& levelsJson, mlg::Entity* entity);

    void LoadBackgroundLevel(const std::string& backgroundPath);
};
