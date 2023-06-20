#include "Scenes/TutorialScene.h"

#include "Core/SceneManager/SceneManager.h"
#include "Core/TimerManager.h"
#include "Gameplay/EntityManager.h"
#include "Managers/LevelTaskManager.h"
#include "Managers/TaskManager.h"
#include "Scenes/LevelScene.h"
#include "Scenes/MenuScene.h"
#include "UI/GameplayOverlay.h"
#include <memory>

TutorialScene::TutorialScene(const std::string& levelPath)
    : LevelScene(levelPath) {
}

void TutorialScene::Load() {
    LevelScene::Load();
    GetLevelTaskManager()->SetDisabled(true);
    mlg::EntityManager::FindByName("ChatManager").lock()->QueueForDeletion();

    TaskData oreTaskData{};
    oreTaskData.productId = "ore";
    oreTaskData.timeLimit = -1.f;
    oreTaskData.reward = 50;
    GetTaskManager()->AddTaskToPool(oreTaskData);
    GetTaskManager()->AcceptNewTask();

    TaskData ironTaskData{};
    ironTaskData.productId = "iron";
    ironTaskData.timeLimit = -1.f;
    ironTaskData.reward = 100;

    for (int i = 0; i < 4; ++i) {
        GetTaskManager()->AddTaskToPool(ironTaskData);
    }

    GetTaskManager()->OnTaskFinished.append(
            [this](const TaskData& taskData) {
                finishedTaskCount++;

                if (finishedTaskCount == 1) {
                    IronTutorial();
                } else if (finishedTaskCount == 2) {
                    AfterIronTutorial();
                } else if (finishedTaskCount == 5) {
                    FinalTutorial();
                }
            });

    messageSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/announcement-sound.mp3");
}

void TutorialScene::Start() {
    mlg::TimerManager::Get()->SetTimer(
            1.f,
            false, [this]() {
                OreTutorial();
            });
}

void TutorialScene::Update() {
    LevelScene::Update();
}

void TutorialScene::OreTutorial() {
    GetGameplayOverlay()->ShowMessage("Ore Tutorial: Bla bla bla", 30.f);
    messageSound->Play();
}

void TutorialScene::IronTutorial() {
    GetTaskManager()->AcceptNewTask();
    GetGameplayOverlay()->ShowMessage("Iron Tutorial: Bla bla bla", 30.f);
    messageSound->Play();
}

void TutorialScene::AfterIronTutorial() {
    for (int i = 0; i < 3; ++i) {
        GetTaskManager()->AcceptNewTask();
    }
    GetGameplayOverlay()->ShowMessage("Post Iron Tutorial: Bla bla bla", 30.f);
    messageSound->Play();
}

void TutorialScene::FinalTutorial() {
    GetGameplayOverlay()->ShowMessage("Finish: You will be exterminated in 3s.", 30.f);
    messageSound->Play();

    mlg::TimerManager::Get()->SetTimer(
            3.f,
            false, [this]() {
                auto scene = std::make_unique<MenuScene>();
                mlg::SceneManager::SetNextScene(std::move(scene));
            });
}
