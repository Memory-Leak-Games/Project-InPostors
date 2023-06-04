#pragma once

#include "Gameplay/Entity.h"

namespace mlg {
    class Transform;
    class Label;
    class ProgressBar;
}

class GameplayOverlay : public mlg::Entity {
private:

    uint32_t timer;
    std::shared_ptr<class mlg::Label> clock;
    std::shared_ptr<class mlg::Label> chat;
    std::shared_ptr<class mlg::Label> score;

    std::shared_ptr<class mlg::ProgressBar> questBar;

    GameplayOverlay(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<GameplayOverlay> Create(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

    void Start() override;
    void Update() override;

    void SetScore(int score);
    void SetChat(const std::string& chat);
    void SetClock(float time);
};
