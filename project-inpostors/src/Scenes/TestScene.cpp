#include "Scenes/TestScene.h"

#include "Core/HID/Input.h"
#include "Core/SceneManager/SceneManager.h"
#include "Gameplay/EntityManager.h"
#include "SceneGraph/SceneGraph.h"
#include "Scenes/LevelScene.h"
#include "TaskManager.h"

#include "ai/AIComponent.h"
#include "ai/Path.h"
#include "ai/SteeringBehaviors.h"
#include "ai/TrafficCar.h"

#include "Levels/LevelGenerator.h"//todo: remove when done testing
#include "ScoreManager.h"

#include "UI/PauseMenu.h"
#include "UI/FinishScreen.h"

#include "Gameplay/EntityManager.h"

TestScene::TestScene() : LevelScene("res/levels/maps/test_level.json") {}

void TestScene::Load() {
    LevelScene::Load();

    TaskManager* taskManager = GetTaskManager();

    std::vector<TaskData> tasks = mlg::LevelGenerator::GetTasks("res/levels/maps/test_level.json");
    for (const auto& task : tasks) {
        taskManager->AddTaskToPool(task);
    }

    auto scoreboard = ScoreManager::GetScoreBoard("TestLevel");
    auto finishScreen = mlg::EntityManager::SpawnEntity<FinishScreen>(
        "FinishScreen", false, mlg::SceneGraph::GetRoot());
}

void TestScene::Update() {
    LevelScene::Update();

    if (mlg::Input::IsActionJustPressed("accept_task")) {
        GetTaskManager()->AcceptNewTask();
    }

    if (mlg::Input::IsActionJustPressed("debug_skip")) {
        auto newScene = std::make_unique<LevelScene>("res/levels/maps/detroit.json");
        mlg::SceneManager::SetNextScene(std::move(newScene));
    }
}
