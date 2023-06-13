#pragma once

class LevelTaskManager {
private:
    size_t newTaskTimer = 0;
    float newTaskProbability = 0.8f;
    int maxActiveTasks = 3;

    std::unique_ptr<class TaskManager> taskManager;
public:
    LevelTaskManager();
    ~LevelTaskManager() = default;

    void StartNewTaskLogic();

    [[nodiscard]] int GetMaxActiveTasks() const;
    void SetMaxActiveTasks(int maxActiveTasks);

    [[nodiscard]] float GetNewTaskProbability() const;
    void SetNewTaskProbability(float newTaskProbability);

    TaskManager& GetTaskManager() const;
};
