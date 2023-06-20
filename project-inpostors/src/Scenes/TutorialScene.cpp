#include "Scenes/TutorialScene.h"

#include "Managers/LevelTaskManager.h"
#include "Managers/TaskManager.h"
#include "Scenes/LevelScene.h"

TutorialScene::TutorialScene(const std::string& levelPath)
    : LevelScene(levelPath) {
}

void TutorialScene::Load() {
    LevelScene::Load();
    GetLevelTaskManager()->SetDisabled(true);
    GetTaskManager()->AcceptNewTask();
}

void TutorialScene::Update() {
}
