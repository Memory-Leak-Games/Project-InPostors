#include "Managers/LevelTaskManager.h"

#include "Core/TimerManager.h"
#include "Managers/TaskManager.h"

#include "effolkronium/random.hpp"
#include <cstddef>
#include <spdlog/spdlog.h>

using Random = effolkronium::random_static;

LevelTaskManager::LevelTaskManager() {
    taskManager = std::make_unique<TaskManager>();
    taskManager->OnTaskFinished.append(
            [this](const TaskData& taskData) {
                tasksCompleted++;
                StartNewTaskLogic();
            });
}

// It starts from 0 to 2 task and set timer to start new task in
// (probability * taskTime) seconds. If there are no free slots for new task,
// it will do nothing. But whole object is waiting for OnTaskFinished event,
// so it will start new task when it will be possible.

void LevelTaskManager::StartNewTaskLogic() {
    if (disabled)
        return;

    if (taskManager->GetTaskCount() == 0)
        return;

    if (taskManager->GetActiveTasksCount() >= GetMaxActiveTasks())
        return;

    int taskCount = taskManager->GetActiveTasksCount();
    int maxTasksToStart = maxActiveTasks - 1 - taskCount;
    int minTasksToStart = taskCount <= 2 ? 1 : 0;
    int tasksToStart = Random::get(minTasksToStart, std::min(2, maxTasksToStart));

    size_t newTaskId = 0;

    for (int i = 0; i < tasksToStart; ++i) {
        newTaskId = taskManager->AcceptNewTask();
    }

    if (newTaskId == 0)
        return;

    const TaskData& newTask = taskManager->GetTask(newTaskId);

    float taskTime = newTask.time;
    uint32_t taskSlots = maxActiveTasks - taskManager->GetActiveTasksCount();
    float timeToNewTask = Random::get(newTaskProbability, 1.0f) * taskTime;

    auto startNewTaskLambda = [this]() {
        StartNewTaskLogic();
    };

    newTaskTimer = mlg::TimerManager::Get()->SetTimer(
            timeToNewTask,
            false,
            startNewTaskLambda);
}

int LevelTaskManager::GetMaxActiveTasks() const {
    return maxActiveTasks;
}

void LevelTaskManager::SetMaxActiveTasks(int maxActiveTasks) {
    this->maxActiveTasks = maxActiveTasks;
}

float LevelTaskManager::GetNewTaskProbability() const {
    return newTaskProbability;
}

void LevelTaskManager::SetNewTaskProbability(float newTaskProbability) {
    this->newTaskProbability = newTaskProbability;
}

TaskManager& LevelTaskManager::GetTaskManager() const {
    return *taskManager;
}

void LevelTaskManager::SetDisabled(bool disabled) {
    this->disabled = disabled;
}

bool LevelTaskManager::IsDisabled() const {
    return disabled;
}