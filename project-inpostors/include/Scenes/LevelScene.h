#pragma once

#include "Audio/Assets/AudioAsset.h"
#include "Core/SceneManager/Scene.h"

class LevelScene : public mlg::Scene {
private:
    const std::string levelPath;

    std::string levelName;

    std::shared_ptr<class FinishScreen> finishScreen;

    std::shared_ptr<mlg::AudioAsset> cityAmbientSound;
    std::shared_ptr<class NavigationGraph> navigationGraph;

    std::shared_ptr<class GameplayOverlay> gameplayOverlay;

    std::shared_ptr<class LevelTaskManager> levelTaskManager;
    std::unique_ptr<class ScoreManager> scoreManager;

    std::weak_ptr<class PauseMenu> pauseMenu;

    size_t timeLimitTimer;

public:
    explicit LevelScene(std::string  path);
    ~LevelScene() override;

    void Load() override;
    void Update() override;

    void HandlePauseGame();

    [[nodiscard]] const std::shared_ptr<NavigationGraph>& GetNavigationGraph() const;
    [[nodiscard]] LevelTaskManager* GetLevelTaskManager();
    [[nodiscard]] class TaskManager* GetTaskManager();
    [[nodiscard]] ScoreManager* GetScoreManager();
    [[nodiscard]] const std::string& GetLevelName() const;

private:
    void InitializeLevelTaskManager();
    void SpawnTraffic();
    void LoadLevel();
    void LoadSounds();
    void SetTimeLimit();
};