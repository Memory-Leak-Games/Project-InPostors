#pragma once

struct TaskData {
    std::string productId;
    float time;
    int reward;
    int bonus;
};

class TaskManager {
private:
    struct Task {
        size_t id;
        std::string productId;
        float timeLeft;
        int reward;
        int bonus;

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
    uint32_t GetActiveTasksCount() const;

    size_t AcceptNewTask();

private:
    void RemoveTask(size_t id);
};
