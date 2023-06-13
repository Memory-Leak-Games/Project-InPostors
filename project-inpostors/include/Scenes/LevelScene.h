#pragma once

#include "Audio/Assets/AudioAsset.h"
#include "Core/SceneManager/Scene.h"
#include "UI/StartLevelCountdown.h"
#include <memory>

class LevelScene : public mlg::Scene {
private:
    const std::string levelPath;

    std::string levelName;

    std::shared_ptr<class FinishScreen> finishScreen;

    std::shared_ptr<class NavigationGraph> navigationGraph;

    std::shared_ptr<class GameplayOverlay> gameplayOverlay;

    std::shared_ptr<class LevelTaskManager> levelTaskManager;
    std::unique_ptr<class ScoreManager> scoreManager;
    std::unique_ptr<class GameplayEventsManager> gameplayEventsManager;
    std::shared_ptr<class AudioManager> audioManager;
    std::shared_ptr<class StartLevelCountdown> levelCountdown;

    std::weak_ptr<class PauseMenu> pauseMenu;

    bool canPlaySound = true;

    size_t timeLimitTimer;

public:
    eventpp::CallbackList<void()> OnLevelFinished;

    explicit LevelScene(std::string  path);
    ~LevelScene() override;

    void Load() override;
    void Update() override;

    void HandlePauseGame();

    [[nodiscard]] const std::shared_ptr<NavigationGraph>& GetNavigationGraph() const;
    [[nodiscard]] LevelTaskManager* GetLevelTaskManager();
    [[nodiscard]] class TaskManager* GetTaskManager();
    [[nodiscard]] ScoreManager* GetScoreManager();
    [[nodiscard]] AudioManager& GetAudioManager();
    [[nodiscard]] GameplayEventsManager& GetGameplayEventsManager();
    [[nodiscard]] StartLevelCountdown& GetLevelCountdown();

    [[nodiscard]] const std::string& GetLevelName() const;
    [[nodiscard]] const std::shared_ptr<class GameplayOverlay>& GetGameplayOverlay() const;
    [[nodiscard]] const std::shared_ptr<class FinishScreen>& GetFinishScreen() const;
    [[nodiscard]] const std::shared_ptr<class PauseMenu>& GetPauseMenu() const;


private:
    void InitializeLevelTaskManager();
    void SpawnTraffic();
    void LoadLevel();
    void SetTimeLimit();
};