#pragma once

#include "Audio/Assets/AudioAsset.h"
#include "Core/SceneManager/Scene.h"

class LevelScene : public mlg::Scene {
private:
    const std::string levelPath;

    std::shared_ptr<mlg::AudioAsset> cityAmbientSound;
public:
    explicit LevelScene(const std::string& path);

    void Load() override;
};