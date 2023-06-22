#include "Scenes/TutorialScene.h"

#include "Core/SceneManager/SceneManager.h"
#include "Core/TimerManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"
#include "Managers/ChatManager.h"
#include "Managers/LevelTaskManager.h"
#include "Managers/TaskManager.h"
#include "Scenes/LevelScene.h"
#include "Scenes/MenuScene.h"
#include "UI/GameplayOverlay.h"
#include <fstream>
#include <memory>

TutorialScene::TutorialScene(const std::string& levelPath)
    : LevelScene(levelPath) {
}

void TutorialScene::Load() {
    LevelScene::Load();
    GetLevelTaskManager()->SetDisabled(true);
    auto chatManagerEntity =
            mlg::EntityManager::FindByName("ChatManager")
                    .lock();

    chatManager = std::dynamic_pointer_cast<ChatManager>(chatManagerEntity);
    chatManager->SetWelcome("welcome-tutorial");
    chatManager->DisableRandomMessages();
    chatManager->DisableTaskMessages();

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
    chatManager->NewMessage("ore-tutorial", 30.f);
}

void TutorialScene::IronTutorial() {
    GetTaskManager()->AcceptNewTask();
    chatManager->NewMessage("iron-tutorial", 30.f);
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

    chatManager->NewMessage("bonus-tutorial", 30.f);
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

    chatManager->NewMessage("after-iron-tutorial", 30.f);
    messageSound->Play();
}

void TutorialScene::FinalTutorial() {
    chatManager->NewMessage("final-tutorial", 10.f);
    messageSound->Play();

    mlg::TimerManager::Get()->SetTimer(
            10.f,
            false, [this]() {
                chatManager->NewMessage("end-tutorial", 10.f);
                messageSound->Play();
            });

    mlg::TimerManager::Get()->SetTimer(
            15.f,
            false, [this]() {
                std::ifstream file(LEVELS_FILE);
                nlohmann::json levels = nlohmann::json::parse(file);
                std::string path = levels["levels"][0]["path"];

                auto scene = std::make_unique<LevelScene>(path);
                mlg::SceneManager::SetNextScene(std::move(scene));
            });
}
