#include "Scenes/TestScene.h"

#include "Core/HID/Input.h"
#include "Gameplay/TaskManager.h"

#include "ai/TrafficCar.h"
#include "ai/AIComponent.h"
#include "ai/SteeringBehaviors.h"
#include "ai/Path.h"

TestScene::TestScene() : LevelScene("res/levels/maps/test_level.json") {}

void TestScene::Load() {
    LevelScene::Load();

    TaskManager* taskManager = GetTaskManager();
    TaskData ironTask {
        "iron",
        5.0f,
        100.0f,
        10.0f,
    };

    TaskData furnitureTask {
        "furniture",
        5.0f,
        100.0f,
        10.0f,
    };

    for (int i = 0; i < 5; ++i) {
        taskManager->AddTaskToPool(ironTask);
        taskManager->AddTaskToPool(furnitureTask);
    }
}

void TestScene::Update() {
    LevelScene::Update();

    if (mlg::Input::IsActionJustPressed("accept_task")) {
        GetTaskManager()->AcceptNewTask();
    }
}
