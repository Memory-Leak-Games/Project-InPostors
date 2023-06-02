#pragma once

#include "Audio/Assets/AudioAsset.h"
#include "Core/SceneManager/Scene.h"
#include "ScoreManager.h"

class LevelScene : public mlg::Scene {
private:
    const std::string levelPath;

    std::string levelName;

    std::shared_ptr<mlg::AudioAsset> cityAmbientSound;
    std::shared_ptr<class NavigationGraph> navigationGraph;

    std::unique_ptr<class TaskManager> taskManager;
    std::unique_ptr<class ScoreManager> scoreManager;

public:
    explicit LevelScene(const std::string& path);
    ~LevelScene() override;

    void Load() override;
    void Update() override;

    void HandlePauseGame();

    [[nodiscard]] const std::shared_ptr<NavigationGraph>& GetNavigationGraph() const;
    [[nodiscard]] TaskManager* GetTaskManager();
    [[nodiscard]] ScoreManager* GetScoreManager();

private:
    void SpawnTraffic();
    void LoadLevel();
    void LoadSounds();
};