#pragma once

#include "Core/SceneManager/Scene.h"

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

    std::shared_ptr<class MessageBox> messageBox;

    std::weak_ptr<class PauseMenu> pauseMenu;

    size_t timeLimitTimer;

protected:
    bool pauseDisabled = false;

public:
    eventpp::CallbackList<void()> OnLevelFinished;

    explicit LevelScene(const std::string& path);
    ~LevelScene() override;

    void Load() override;
    void Start() override;
    void Update() override;

    void HandlePauseGame();

    [[nodiscard]] const std::shared_ptr<NavigationGraph>& GetNavigationGraph() const;
    [[nodiscard]] class TaskManager* GetTaskManager();
    [[nodiscard]] LevelTaskManager* GetLevelTaskManager();
    [[nodiscard]] ScoreManager* GetScoreManager();
    [[nodiscard]] AudioManager& GetAudioManager();
    [[nodiscard]] MessageBox& GetMessageBox();
    [[nodiscard]] GameplayEventsManager& GetGameplayEventsManager();
    [[nodiscard]] StartLevelCountdown& GetLevelCountdown();

    [[nodiscard]] const std::string& GetLevelName() const;
    [[nodiscard]] GameplayOverlay* GetGameplayOverlay();
    [[nodiscard]] const std::shared_ptr<class FinishScreen>& GetFinishScreen() const;
    [[nodiscard]] const std::shared_ptr<class PauseMenu>& GetPauseMenu() const;

    [[nodiscard]] const std::string& GetLevelPath() const;


private:
    void InitializeLevelTaskManager();
    void SpawnTraffic();
    void LoadLevel();
    void SetTimeLimit();

    void InitializeMessageBox();
    void InitializeGamepadCallbacks();
};