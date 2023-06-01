#pragma once

#include "Audio/Assets/AudioAsset.h"
#include "Core/SceneManager/Scene.h"
#include "Levels/NavigationGraph.h"
#include <memory>

class LevelScene : public mlg::Scene {
private:
    const std::string levelPath;

    std::shared_ptr<mlg::AudioAsset> cityAmbientSound;

    std::shared_ptr<NavigationGraph> navigationGraph;
public:
    explicit LevelScene(const std::string& path);

    void Load() override;
    void Update() override;

    [[nodiscard]] const std::shared_ptr<NavigationGraph>& GetNavigationGraph() const;
};