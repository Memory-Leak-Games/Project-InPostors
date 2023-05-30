#pragma once

#include "Levels/NavigationGraph.h"
#include "Scenes/LevelScene.h"

class TestScene : public LevelScene {
private:
    std::shared_ptr<NavigationGraph> navigationGraph;
public:
    explicit TestScene();

    void Load() override;
    void Update() override;
};