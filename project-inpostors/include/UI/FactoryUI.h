#pragma once

#include "Gameplay/SceneComponent.h"
#include <memory>
#include <vector>

namespace mlg {
    class Entity;
    class Image;
    class ProgressBar;
    class CanvasPanel;
}// namespace mlg

class FactoryUI : public mlg::SceneComponent {

    std::shared_ptr<class Factory> factory;

    std::shared_ptr<mlg::Image> uiPin;
    std::shared_ptr<mlg::ProgressBar> uiProgress;
    std::shared_ptr<mlg::Image> uiInputBars[2];
    std::shared_ptr<mlg::Image> uiIcon;
    std::shared_ptr<mlg::Image> arrow;
    std::vector<std::shared_ptr<mlg::Image>> requiredIcons;
    std::vector<std::shared_ptr<mlg::Image>> requiredPanels;

public:
    FactoryUI(const std::weak_ptr<class mlg::Entity>& owner, const std::string& name);

    void Start() override;
    void Update() override;

private:
    void UpdateFactoryInputIcons();
    void AnimatePanel();


};
