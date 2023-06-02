#pragma once

#include "Levels/NavigationGraph.h"
#include "Scenes/LevelScene.h"

class TestScene : public LevelScene {
public:
    explicit TestScene();

    void Load() override;
};