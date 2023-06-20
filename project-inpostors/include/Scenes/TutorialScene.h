#pragma once

#include "Scenes/LevelScene.h"

class TutorialScene : public LevelScene {
private:
public:
    explicit TutorialScene(const std::string& levelPath);

    void Load() override;
    void Update() override;
};