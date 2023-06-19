#include "Managers/TaskManager.h"
#include "Core/TimerManager.h"
#include "Managers/ScoreManager.h"
#include "Utils/ProductManager.h"
#include <cstddef>
#include <spdlog/spdlog.h>
#include <vector>

using Random = effolkronium::random_static;

void TaskManager::Update() {
    float deltaTime = mlg::Time::GetDeltaSeconds();

    for (auto& [id, task] : tasks) {
        if (!task.active)
            continue;

        task.timeLeft -= deltaTime;

        if (task.timeLeft <= 0)
            OnTaskFailed(GetTask(id));
    }
}

void TaskManager::AddTaskToPool(const TaskData& newTaskData) {
    Task newTask{};
    newTask.id = nextId++;
    newTask.productId = newTaskData.productId;
    newTask.timeLeft = newTaskData.time;
    newTask.timeLimit = newTaskData.time;
    newTask.reward = newTaskData.reward;
    newTask.bonus = newTaskData.bonus;

    tasks.emplace(newTask.id, newTask);
}

bool TaskManager::FinishTask(const std::string& productId) {
    auto timeComparator = [](const Task* a, const Task* b) {
        return a->timeLeft < b->timeLeft;
    };

    std::set<const Task*, decltype(timeComparator)> activeProductTasks;

    // Find all active tasks for these products
    for (const auto& [id, task] : tasks) {
        if (task.productId == productId && task.active) {
            activeProductTasks.insert(&task);
        }
    }

    // If there are no active tasks for this product, do nothing
    if (activeProductTasks.empty())
        return false;

    // Select the task with the least time left but if there are tasks that
    // have not already ended, select the oldest one
    const Task* taskToFinish = *activeProductTasks.begin();
    auto oldestTaskButNotEnded = std::ranges::find_if(
            activeProductTasks,
            [this](const Task* task) {
                return task->timeLeft > 0;
            });

    if (oldestTaskButNotEnded != activeProductTasks.end()) {
        taskToFinish = *oldestTaskButNotEnded;
    }

    RemoveTask(taskToFinish->id);
    return true;
}

void TaskManager::SellProduct(const std::string& productId) {
    const Product& product = ProductManager::Get()->GetProduct(productId);
    OnProductSold(product.price);
}

bool TaskManager::HasTask(const std::string& productId) {
    std::vector<size_t> activeProductTasks = GetActiveTasksIds();

    return std::ranges::any_of(
            activeProductTasks,
            [this, &productId](const size_t id) {
                return tasks[id].productId == productId;
            });
}

TaskData TaskManager::GetTask(size_t id) {
    const Task& task = tasks.at(id);

    return TaskData{
            task.productId,
            task.timeLeft,
            task.timeLimit,
            task.reward,
            task.bonus};
}

std::vector<TaskData> TaskManager::GetActiveTasks() {
    std::vector<TaskData> activeTasks;

    for (const auto& [id, task] : tasks) {
        if (task.active) {
            activeTasks.push_back(TaskData{
                    task.productId,
                    task.timeLeft > 0 ? task.timeLeft : 0,
                    task.timeLimit,
                    task.reward,
                    task.bonus});
        }
    }

    return activeTasks;
}

std::vector<TaskData> TaskManager::GetActiveTasks(const std::string& productId) {
    std::vector<TaskData> activeTasks = GetActiveTasks();

    // Remove tasks that don't match the product id
    activeTasks.erase(
            std::remove_if(
                    activeTasks.begin(),
                    activeTasks.end(),
                    [&productId](const TaskData& task) {
                        return task.productId != productId;
                    }),
            activeTasks.end());

    return activeTasks;
}

uint32_t TaskManager::GetActiveTasksCount() const {
    return std::count_if(
            tasks.begin(), tasks.end(),
            [](const auto& task) {
                return task.second.active;
            });
}

size_t TaskManager::AcceptNewTask() {
    std::vector<size_t> inactiveTasks;

    for (const auto& [id, task] : tasks) {
        if (!task.active) {
            inactiveTasks.push_back(id);
        }
    }

    // If there are no inactive tasks, do nothing
    if (inactiveTasks.empty())
        return 0;

    size_t newTask = *Random::get(inactiveTasks);
    tasks[newTask].active = true;
    OnTaskAccepted(GetTask(newTask));

    return newTask;
}

void TaskManager::RemoveTask(size_t id) {
    const Task& task = tasks.at(id);

    if (!task.active)
        return;

    TaskData taskData{
            task.productId,
            task.timeLeft,
            task.timeLimit,
            task.reward,
            task.bonus};

    OnTaskFinished(taskData);

    tasks.erase(id);
}

std::vector<size_t> TaskManager::GetActiveTasksIds() {
    std::vector<size_t> ids;

    for (const auto& [id, task] : tasks) {
        if (task.active) {
            ids.push_back(id);
        }
    }

    return ids;
}
