#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class Entity;
    class Image;
}

class PlayerUI : public mlg::Component {

    std::shared_ptr<class Player> player;

    std::weak_ptr<class mlg::Image> uiArrow;
    std::shared_ptr<class mlg::Image> eqBillboards[3];
    std::shared_ptr<class mlg::Image> eqIcons[3];

public:
    PlayerUI(const std::weak_ptr<class mlg::Entity>& owner, const std::string& name, const std::shared_ptr<class Player>& player);
    void Start() override;
    void Update() override;
};
