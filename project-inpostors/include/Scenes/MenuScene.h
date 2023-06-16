#pragma once

#include "Core/SceneManager/Scene.h"

class MenuScene : public mlg::Scene {
public:
    explicit MenuScene() = default;
    ~MenuScene() override;

    void Load() override;
};