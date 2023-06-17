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

public:
    explicit MenuScene() = default;
    ~MenuScene() override;

    void Load() override;

private:
    void InitializeMainMenu();
    void BindToOnExit(mlg::Button& button);
    void BindToOnPlay(mlg::Button& button);
    void BindToOnCredits(mlg::Button& button);

    void InitializeCredits();
    void BindToOnCreditsBack(mlg::Button& button);
};
