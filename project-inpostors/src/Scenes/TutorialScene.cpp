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
    GetTaskManager()->AddTaskToPool(ironTaskData);

    GetTaskManager()->OnTaskFinished.append(
            [this](const TaskData& taskData) {
                finishedTaskCount++;

                if (finishedTaskCount == 1) {
                    IronTutorial();
                } else if (finishedTaskCount == 2) {
                    BonusTutorial();
                } else if (finishedTaskCount == 3) {
                    AfterIronTutorial();
                } else if (finishedTaskCount == 6) {
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
    GetGameplayOverlay()->ShowMessage("Ore Task: To complete your first assignment pick up the ore from "
                                      "the factory and deliver it to the main storage.", 30.f);
    messageSound->Play();
}

void TutorialScene::IronTutorial() {
    GetTaskManager()->AcceptNewTask();
    GetGameplayOverlay()->ShowMessage("Iron Tutorial: Now for something more complex. "
                                      "When you deliver products to some factories they can convert them into other products, "
                                      "you can try that out by delivering ore to the iron factory ;)", 30.f);
    messageSound->Play();
}

void TutorialScene::BonusTutorial() {
    TaskData ironTaskDataWithLongBonus{};
    ironTaskDataWithLongBonus.productId = "iron";
    ironTaskDataWithLongBonus.timeLimit = -1.f;
    ironTaskDataWithLongBonus.time = 600.f;
    ironTaskDataWithLongBonus.reward = 100;
    ironTaskDataWithLongBonus.bonus = 50;
    GetTaskManager()->AddTaskToPool(ironTaskDataWithLongBonus);

    GetTaskManager()->AcceptNewTask();
    GetGameplayOverlay()->ShowMessage("Bonus Tutorial: We put a strong emphasis on saving our customer's time, "
                                      "that's why if you manage to deliver required products to the storage quickly, "
                                      "you might receive some bonus cash!", 30.f);
    messageSound->Play();
}

void TutorialScene::AfterIronTutorial() {
    TaskData ironTaskDataWithNormalBonus{};
    ironTaskDataWithNormalBonus.productId = "iron";
    ironTaskDataWithNormalBonus.timeLimit = -1.f;
    ironTaskDataWithNormalBonus.time = 45.f;
    ironTaskDataWithNormalBonus.reward = 100;
    ironTaskDataWithNormalBonus.bonus = 50;

    TaskData oreTaskDataWithNormalBonus{};
    oreTaskDataWithNormalBonus.productId = "ore";
    oreTaskDataWithNormalBonus.timeLimit = -1.f;
    oreTaskDataWithNormalBonus.time = 20.f;
    oreTaskDataWithNormalBonus.reward = 50;
    oreTaskDataWithNormalBonus.bonus = 25;
    GetTaskManager()->AddTaskToPool(oreTaskDataWithNormalBonus);
    GetTaskManager()->AcceptNewTask();

    for (int i = 0; i < 2; ++i) {
        GetTaskManager()->AddTaskToPool(ironTaskDataWithNormalBonus);
        GetTaskManager()->AcceptNewTask();
    }

    GetGameplayOverlay()->ShowMessage("Now for your last assignment: You have 3 tasks to complete. "
                                      "If you got through all the previous steps this shouldn't be a problem.", 30.f);
    messageSound->Play();
}

void TutorialScene::FinalTutorial() {
    GetGameplayOverlay()->ShowMessage("Finish: You have successfully completed your test assignment, "
                                      "which means you are worthy of working under me! "
                                      "I want to officially welcome you as a new AiPost courier!", 10.f);
    messageSound->Play();

    mlg::TimerManager::Get()->SetTimer(
            10.f,
            false, [this]() {
                GetGameplayOverlay()->ShowMessage("Now you will be exterminated... yhym moved to a new city shortly.", 10.f);
                messageSound->Play();
            });

    mlg::TimerManager::Get()->SetTimer(
            15.f,
            false, [this]() {
                auto scene = std::make_unique<MenuScene>();
                mlg::SceneManager::SetNextScene(std::move(scene));
            });
}
