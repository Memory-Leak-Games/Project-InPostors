#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class Entity;
    class Image;
    class ProgressBar;
}

class FactoryUI : public mlg::Component {

    std::shared_ptr<class Factory> factory;

    std::shared_ptr<class mlg::Image> uiPin;
    std::shared_ptr<class mlg::ProgressBar> uiProgress;
    std::shared_ptr<class mlg::Image> uiInputBars[2];
    std::shared_ptr<class mlg::Image> uiIcon;

public:
    FactoryUI(const std::weak_ptr<class mlg::Entity>& owner, const std::string& name, const std::shared_ptr<Factory>& factory);

    void Update() override;

    void UpdateFactoryInputIcons();
};
