#pragma once

#include <cstddef>
class LevelTaskManager {
private:
    size_t newTaskTimer = 0;

    int tasksCompleted = 0;
    
    float newTaskProbability = 0.8f;
    int maxActiveTasks = 4;

    std::unique_ptr<class TaskManager> taskManager;

    bool disabled = false;
public:
    LevelTaskManager();
    ~LevelTaskManager() = default;

    void StartNewTaskLogic();

    [[nodiscard]] int GetMaxActiveTasks() const;
    void SetMaxActiveTasks(int maxActiveTasks);

    [[nodiscard]] float GetNewTaskProbability() const;
    void SetNewTaskProbability(float newTaskProbability);

    TaskManager& GetTaskManager() const;
    
    void SetDisabled(bool disabled);
    [[nodiscard]] bool IsDisabled() const;

};
