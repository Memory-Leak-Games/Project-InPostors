#include "Scenes/TestScene.h"

#include "Core/HID/Input.h"
#include "TaskManager.h"

#include "ai/TrafficCar.h"
#include "ai/AIComponent.h"
#include "ai/SteeringBehaviors.h"
#include "ai/Path.h"

#include "ScoreManager.h"
#include <spdlog/spdlog.h>

TestScene::TestScene() : LevelScene("res/levels/maps/test_level.json") {}

void TestScene::Load() {
    LevelScene::Load();

    TaskManager* taskManager = GetTaskManager();
    TaskData ironTask {
        "iron",
        20.0f,
        100,
        10,
    };

    TaskData furnitureTask {
        "furniture",
        5.0f,
        100,
        10,
    };

    for (int i = 0; i < 5; ++i) {
        taskManager->AddTaskToPool(ironTask);
        // taskManager->AddTaskToPool(furnitureTask);
    }

    auto scoreboard = ScoreManager::GetScoreBoard("TestLevel");
    for (const auto& [name, score] : scoreboard) {
        SPDLOG_INFO("{}: {}", name, score);
    }

    ScoreManager::SaveScore("TestLevel", "Beans", 100);
    ScoreManager::SaveScore("MLGLevel", "_MLG_", 2137);


}

void TestScene::Update() {
    LevelScene::Update();

    if (mlg::Input::IsActionJustPressed("accept_task")) {
        GetTaskManager()->AcceptNewTask();
    }
}
