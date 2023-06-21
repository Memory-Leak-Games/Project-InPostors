#pragma once

#include "Gameplay/Component.h"
#include <memory>

namespace mlg {
    class Entity;
    class Image;
    class Label;
    class CanvasPanel;
}

class PlayerUI : public mlg::Component {

    std::shared_ptr<class Player> player;

    std::weak_ptr<mlg::Image> uiArrow;

    std::weak_ptr<mlg::CanvasPanel> eqPanel;
    std::vector<std::shared_ptr<mlg::Image>> eqIcons;

    std::shared_ptr<mlg::Image> eqBillboards[3];
    std::shared_ptr<mlg::Label> startLabel;

    unsigned int startTimerHandle;

public:
    PlayerUI(const std::weak_ptr<class mlg::Entity>& owner, const std::string& name);
    void Start() override;
    void Update() override;
};
