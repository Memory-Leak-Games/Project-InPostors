#pragma once

#include "Core/SceneManager/Scene.h"
#include "UI/Components/Containers/CanvasPanel.h"
#include "UI/Components/Label.h"
#include <array>
#include <memory>

namespace mlg {
    class Container;
    class CanvasPanel;
    class Image;
    class Button;
    class Label;
}// namespace mlg

class MenuScene : public mlg::Scene {
private:
    std::weak_ptr<mlg::CanvasPanel> mainMenuContainer;
    std::weak_ptr<mlg::CanvasPanel> creditsContainer;
    std::weak_ptr<mlg::CanvasPanel> settingsContainer;

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
    void BindToOnApply(mlg::Button& button);

    void BindToBackToMainMenu(mlg::Button& button, mlg::Container& container);
};
