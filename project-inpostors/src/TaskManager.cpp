#include "TaskManager.h"
#include "Core/TimerManager.h"

using Random = effolkronium::random_static;

void TaskManager::Update() {
    float deltaTime = mlg::Time::GetDeltaSeconds();

    for (auto& [id, task] : tasks) {
        if (!task.active)
            continue;

        task.timeLeft -= deltaTime;
    }
}

void TaskManager::AddTaskToPool(const TaskData& newTaskData) {
    Task newTask{};
    newTask.id = nextId++;
    newTask.productId = newTaskData.productId;
    newTask.timeLeft = newTaskData.time;
    newTask.reward = newTaskData.reward;
    newTask.bonus = newTaskData.bonus;

    tasks.emplace(newTask.id, newTask);
}

void TaskManager::FinishTask(const std::string& productId) {
    std::vector<size_t> activeProductTasks;

    for (const auto& [id, task] : tasks) {
        if (task.productId == productId && task.active) {
            activeProductTasks.push_back(id);
        }
    }

    // If there are no active tasks for this product, do nothing
    if (activeProductTasks.empty())
        return;

    auto timeComparator = [this](const size_t a, const size_t b) {
        return tasks[a].timeLeft < tasks[b].timeLeft;
    };

    size_t taskToFinish = std::min_element(
            activeProductTasks.begin(),
            activeProductTasks.end(),
            timeComparator)[0];

    RemoveTask(taskToFinish);
}

TaskData TaskManager::GetTask(size_t id) {
    const Task& task = tasks.at(id);

    return TaskData{
            task.productId,
            task.timeLeft,
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
    return newTask;
}

void TaskManager::RemoveTask(size_t id) {
    const Task& task = tasks.at(id);

    if (!task.active)
        return;

    TaskData taskData{
            task.productId,
            task.timeLeft,
            task.reward,
            task.bonus};

    OnTaskFinished(taskData);

    tasks.erase(id);
}
