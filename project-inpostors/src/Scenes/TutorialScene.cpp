#include "Scenes/TutorialScene.h"

#include <fstream>

#include "Audio/Assets/AudioAsset.h"
#include "Core/SceneManager/SceneManager.h"
#include "Core/TimerManager.h"

#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"

#include "Managers/ChatManager.h"
#include "Managers/LevelTaskManager.h"
#include "Managers/TaskManager.h"
#include "UI/StartLevelCountdown.h"

#include "Scenes/LevelScene.h"
#include "Scenes/MenuScene.h"

#include "UI/GameplayOverlay.h"
#include "UI/TutorialPanel.h"

TutorialScene::TutorialScene(const std::string& levelPath)
    : LevelScene(levelPath) {
}

void TutorialScene::Load() {
    LevelScene::Load();
    GetLevelTaskManager()->SetDisabled(true);
    auto chatManagerEntity =
            mlg::EntityManager::FindByName("ChatManager")
                    .lock();

    tutorialPanel = mlg::EntityManager::SpawnEntity<TutorialPanel>(
                            "TutorialPanel", false, nullptr)
                            .lock();

    chatManager = std::dynamic_pointer_cast<ChatManager>(chatManagerEntity);
    chatManager->DisableTaskMessages();
    chatManager->DisableWelcomeMessage();


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

    tutorialPanel->OnClosed.append(
            [this]() {
                pauseDisabled = false;
            });
    tutorialPanel->OnMessage.append(
            [this]() {
                pauseDisabled = true;
                messageSound->Play();
            });
}

void TutorialScene::Start() {
    auto tutorials = std::vector<std::string>{
            "welcome-tutorial",
            "post-welcome-tutorial",
    };

    tutorialPanel->ShowTutorials(tutorials);

    using CL = eventpp::CallbackList<void()>;
    CL::Handle eventHandle = tutorialPanel->OnClosed.append(
            [this]() {
                GetLevelCountdown().StartCountDown();
            });

    mlg::TimerManager::Get()->SetTimer(
            3.f,
            false, [this, eventHandle]() {
                OreTutorial();
                tutorialPanel->OnClosed.remove(eventHandle);
            });
}

void TutorialScene::Update() {
    LevelScene::Update();
}

void TutorialScene::OreTutorial() {
    tutorialPanel->ShowTutorial("ore-tutorial");
}

void TutorialScene::IronTutorial() {
    GetTaskManager()->AcceptNewTask();
    tutorialPanel->ShowTutorial("iron-tutorial");
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

    tutorialPanel->ShowTutorial("bonus-tutorial");
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

    tutorialPanel->ShowTutorial("after-iron-tutorial");
}

void TutorialScene::FinalTutorial() {
    tutorialPanel->ShowTutorial("final-tutorial");

    mlg::TimerManager::Get()->SetTimer(
            0.5f,
            false, [this]() {
                std::ifstream file(LEVELS_FILE);
                nlohmann::json levels = nlohmann::json::parse(file);
                std::string path = levels["levels"][0]["path"];

                auto scene = std::make_unique<LevelScene>(path);
                mlg::SceneManager::SetNextScene(std::move(scene));
            });
}
