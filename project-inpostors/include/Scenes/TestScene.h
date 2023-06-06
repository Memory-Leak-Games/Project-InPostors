#pragma once

#include "Scenes/LevelScene.h"

class TestScene : public LevelScene {
private:
public:
    explicit TestScene();

    void Load() override;
    void Update() override;
};