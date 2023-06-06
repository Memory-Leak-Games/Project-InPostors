#pragma once

#include "Scenes/LevelScene.h"

class TestScene : public LevelScene {
private:
    std::weak_ptr<class PauseMenu> pauseMenu;
public:
    explicit TestScene();

    void Load() override;
    void Update() override;
};