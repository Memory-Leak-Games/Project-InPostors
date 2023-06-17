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
    std::weak_ptr<mlg::Container> mainMenuContainer;
    std::weak_ptr<mlg::Image> menuBackground;

    std::weak_ptr<mlg::CanvasPanel> creditsContainer;

public:
    explicit MenuScene() = default;
    ~MenuScene() override;

    void Load() override;

private:
    void InitializeMainMenu();
    void BindToOnExit(const std::shared_ptr<mlg::Button>& button);

    void InitializeCredits();
};
