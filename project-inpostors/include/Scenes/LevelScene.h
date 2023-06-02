#pragma once

#include "Audio/Assets/AudioAsset.h"
#include "Core/SceneManager/Scene.h"
#include "Gameplay/TaskManager.h"
#include "Levels/NavigationGraph.h"

class LevelScene : public mlg::Scene {
private:
    const std::string levelPath;

    std::shared_ptr<mlg::AudioAsset> cityAmbientSound;
    std::shared_ptr<NavigationGraph> navigationGraph;

    std::unique_ptr<TaskManager> taskManager;

public:
    explicit LevelScene(const std::string& path);

    void Load() override;
    void Update() override;

    void HandlePauseGame();

    [[nodiscard]] const std::shared_ptr<NavigationGraph>& GetNavigationGraph() const;
    [[nodiscard]] TaskManager* GetTaskManager();

private:
    void SpawnTraffic();
    void LoadLevel();
    void LoadSounds();
};