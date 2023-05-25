#pragma once

#include "Gameplay/Entity.h"

namespace mlg {
    class Transform;
    class Label;
}

class GameplayOverlay : public mlg::Entity {
private:

    uint32_t timer;
    std::shared_ptr<class mlg::Label> clock;
    std::shared_ptr<class mlg::Label> chat;
    std::shared_ptr<class mlg::Label> score;

    GameplayOverlay(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<GameplayOverlay> Create(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

    void Start() override;
    void Update() override;
};
