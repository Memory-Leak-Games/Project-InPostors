#pragma once

#include "Gameplay/Entity.h"

#define TASK_PANELS 5

namespace mlg {
    class Transform;
    class Image;
    class Label;
    class ProgressBar;
}// namespace mlg

class GameplayOverlay : public mlg::Entity {
private:
    uint32_t timer;
    std::shared_ptr<class mlg::Label> clock;
    std::shared_ptr<class mlg::Label> chat;
    std::shared_ptr<class mlg::Label> score;

    std::shared_ptr<class mlg::Image> taskPanel[TASK_PANELS];
    std::shared_ptr<class mlg::Image> taskIcon[TASK_PANELS];
    std::shared_ptr<class mlg::ProgressBar> taskProgress[TASK_PANELS];
    std::shared_ptr<class mlg::Image> taskRequired[TASK_PANELS][2];
    std::shared_ptr<class mlg::Image> taskRequiredPanel[TASK_PANELS][2];

    std::shared_ptr<class LevelTaskManager> taskManager;

    GameplayOverlay(uint64_t id,
                    const std::string& name,
                    bool isStatic,
                    mlg::Transform* parent);

public:
    static std::shared_ptr<GameplayOverlay> Create(
            uint64_t id,
            const std::string& name,
            bool isStatic,
            mlg::Transform* parent,
            std::shared_ptr<class LevelTaskManager> taskManager);

    void Start() override;
    void Update() override;

    void SetScore(int score);
    void SetChat(const std::string& chat);
    void SetClock(float time);
    void UpdateTask(int idx);
};
