#pragma once

#include "Scenes/LevelScene.h"

namespace mlg {
    class AudioAsset;
}

class TutorialScene : public LevelScene {
private:
    int finishedTaskCount = 0;
    std::shared_ptr<mlg::AudioAsset> messageSound;
    std::shared_ptr<class ChatManager> chatManager;
    std::shared_ptr<class TutorialPanel> tutorialPanel;

public:
    explicit TutorialScene(const std::string& levelPath);

    void Load() override;
    void Start() override;
    void Update() override;

private:
    void OreTutorial();
    void IronTutorial();
    void BonusTutorial();
    void AfterIronTutorial();
    void FinalTutorial();
};