#pragma once

#include <cstddef>
#include <eventpp/callbacklist.h>
struct TaskData {
    std::string productId;
    float time;
    float reward;
    float bonus;
};

class TaskManager {
private:
    struct Task {
        size_t id;
        std::string productId;
        float timeLeft;
        float reward;
        float bonus;

        bool active = false;
    };

    std::unordered_map<size_t, Task> tasks;
    size_t nextId = 1;

public:
    eventpp::CallbackList<void(const TaskData&)> OnTaskFinished;
    eventpp::CallbackList<void(const TaskData&)> OnTaskAccepted;

    TaskManager() = default;
    ~TaskManager() = default;

    void Update();

    void AddTaskToPool(const TaskData& taskData);
    void FinishTask(const std::string& productId);

    TaskData GetTask(size_t id);

    std::vector<TaskData> GetActiveTasks();
    std::vector<TaskData> GetActiveTasks(const std::string& productId);

    size_t AcceptNewTask();

private:
    void RemoveTask(size_t id);
};
