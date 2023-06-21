#pragma once

#include "Gameplay/Entity.h"
#include <cstddef>
#include <memory>

#define TASK_PANELS 5

namespace mlg {
    class Transform;
    class Image;
    class Label;
    class ProgressBar;
    class CanvasPanel;
}// namespace mlg

class GameplayOverlay : public mlg::Entity {
private:
    uint32_t timer;
    std::shared_ptr<mlg::Label> clock;
    std::shared_ptr<mlg::Label> chat;
    std::shared_ptr<mlg::CanvasPanel> chatWindow;
    std::shared_ptr<mlg::Label> score;

    std::shared_ptr<class mlg::Image> taskPanel[TASK_PANELS];
    std::shared_ptr<class mlg::Image> taskIcon[TASK_PANELS];
    std::shared_ptr<class mlg::ProgressBar> taskProgress[TASK_PANELS];
    std::shared_ptr<class mlg::Image> taskProgressBg[TASK_PANELS];
    std::shared_ptr<class mlg::Image> taskRequired[TASK_PANELS][2];
    std::shared_ptr<class mlg::Label> taskReward[TASK_PANELS];
    std::shared_ptr<class mlg::Image> taskPlus[TASK_PANELS];
    std::shared_ptr<class mlg::Label> taskBonus[TASK_PANELS];

    size_t chatTimer = 0;

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

    void ShowMessage(const std::string& message, float visibleTime = 5.f);
    bool IsChatVisible() const;

    void SetClock(float time);
    void UpdateTask(int idx);

    void SetTaskVisible(int idx, bool visible);
};
