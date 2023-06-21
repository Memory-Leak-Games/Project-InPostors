#pragma once

#include "Scenes/LevelScene.h"

class TutorialScene : public LevelScene {
private:
    int finishedTaskCount = 0;
    std::shared_ptr<mlg::AudioAsset> messageSound;
public:
    explicit TutorialScene(const std::string& levelPath);

    void Start() override;
    void Load() override;
    void Update() override;

private: 
    void OreTutorial();
    void IronTutorial();
    void BonusTutorial();
    void AfterIronTutorial();
    void FinalTutorial();
};