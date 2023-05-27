#pragma once

#include "Core/SceneManager/Scene.h"

class LevelScene : public mlg::Scene {
private:
    const std::string levelPath;

public:
    explicit LevelScene(const std::string& path);

    void Load() override;
};