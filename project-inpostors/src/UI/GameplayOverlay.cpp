#include "../../include/UI/GameplayOverlay.h"
#include "Core/RGBA.h"
#include "SceneGraph/Transform.h"

#include "Core/Time.h"
#include "Core/TimerManager.h"
#include "Managers/LevelTaskManager.h"
#include "Managers/TaskManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/Containers/CanvasPanel.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/ProgressBar.h"
#include "Utils/BlueprintManager.h"
#include "Utils/ProductManager.h"
#include <spdlog/fmt/bundled/format.h>
#include <string>

GameplayOverlay::GameplayOverlay(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent)
    : mlg::Entity(id, name, isStatic, parent) {}

std::shared_ptr<GameplayOverlay> GameplayOverlay::Create(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent,
                                                         std::shared_ptr<class LevelTaskManager> taskManager) {
    auto result = std::shared_ptr<GameplayOverlay>(new GameplayOverlay(id, name, isStatic, parent));

    result->taskManager = taskManager;

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay_background_material.json");
    auto temp = result->AddComponent<mlg::Image>("ClockPanel", material).lock();
    temp->SetSize({110, 48});
    temp->SetPosition({640, 720 - 8 - 24});
    temp->SetAnchor({0.5, 1.0});

    result->clock = result->AddComponent<mlg::Label>("Clock").lock();
    result->clock->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
    result->clock->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
    result->clock->SetPosition({640.f, 720.f - 24 - 2 - 8.f});
    result->clock->SetAnchor({0.5, 1.0});

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay_background_material.json");
    temp = result->AddComponent<mlg::Image>("ScorePanel", material).lock();
    temp->SetSize({110.f, 48.f});
    temp->SetPosition({1280.f - 50.f - 8.f, 720.f - 8.f - 24.f});
    temp->SetAnchor({1.0, 1.0});

    result->score = result->AddComponent<mlg::Label>("Score").lock();
    result->score->SetPosition({1280.f - 50.f - 8.f, 720.f - 8.f - 24.f - 2});
    result->score->SetAnchor({1.0, 1.0});
    result->score->SetText("$0000");
    result->score->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
    result->score->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);


    // chat window
    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay_background_material.json");
    result->chatWindow = result->AddComponent<mlg::CanvasPanel>("ChatWindow").lock();
    result->chatWindow->SetPosition({1280.f - 230.f, 95.f});
    result->chatWindow->SetAnchor({1.f, 0.f});

    auto ui = result->AddComponent<mlg::Image>("ChatWindow", material).lock();
    ui->SetSize({400, 125});
    ui->tint = glm::vec4(0.7f, 0.7f, 0.7f, 0.9f);
    result->chatWindow->AddChild(ui);

    ui = result->AddComponent<mlg::Image>("ChatWindow", material).lock();
    ui->SetAnchor({0.5, 0});
    ui->SetSize({400 - 10, 125 - 10});
    ui->tint = glm::vec4(0.f, 0.f, 0.f, 0.95f);
    result->chatWindow->AddChild(ui);

    result->chat = result->AddComponent<mlg::Label>("Chat").lock();
    result->chat->SetRelativePosition({-190.f, 40.f});
    result->chat->SetSize(18);
    result->chatWindow->AddChild(result->chat);

    result->chatWindow->SetVisible(false);

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/task_panel_material.json");
    for (int i = 0; i < TASK_PANELS; i++) {
        result->taskPanel[i] = result->AddComponent<mlg::Image>("TaskPanel", material).lock();
        result->taskPanel[i]->SetPosition({8 + 48 + i * (96.f + 8.f), 720 - 48});
        result->taskPanel[i]->SetAnchor({0.0, 1.0});
        result->taskPanel[i]->SetSize({96, 96});
        result->taskPanel[i]->SetVisible(false);
    }

    // Bonus
    for (int i = 0; i < TASK_PANELS; i++) {
        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/task_progress_bg_material.json");
        result->taskProgressBg[i] = result->AddComponent<mlg::Image>("TaskProgressBg", material).lock();
        result->taskProgressBg[i]->SetPosition(result->taskPanel[i]->GetPosition() + glm::vec2(0.f, -50.f));
        result->taskProgressBg[i]->SetAnchor({0.0, 1.0});
        result->taskProgressBg[i]->SetSize({70.f, 28.f});
        result->taskProgressBg[i]->SetVisible(false);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/task_progress_material.json");
        result->taskProgress[i] = result->AddComponent<mlg::ProgressBar>("TaskProgress", material).lock();
        result->taskProgress[i]->SetPosition(result->taskPanel[i]->GetPosition() + glm::vec2(4.f, -50.f));
        result->taskProgress[i]->SetAnchor({0.0, 1.0});
        result->taskProgress[i]->SetSize({56.f, 22.f});
        result->taskProgress[i]->percentage = 0.0f;
        result->taskProgress[i]->SetVisible(false);

        material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/plus_material.json");
        result->taskPlus[i] = result->AddComponent<mlg::Image>("TaskProgressBg", material).lock();
        result->taskPlus[i]->SetPosition(result->taskPanel[i]->GetPosition() + glm::vec2(-30.f, -50.f));
        result->taskPlus[i]->SetAnchor({0.0, 1.0});
        result->taskPlus[i]->SetSize({32.f, 32.f});
        result->taskPlus[i]->SetVisible(false);

        result->taskBonus[i] = result->AddComponent<mlg::Label>("TaskBonus").lock();
        result->taskBonus[i]->SetPosition(result->taskPanel[i]->GetPosition() + glm::vec2(10.f, -51.f));
        result->taskBonus[i]->SetAnchor({0.0, 1.0});
        result->taskBonus[i]->SetSize(20);
        result->taskBonus[i]->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
        result->taskBonus[i]->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
        result->taskBonus[i]->SetVisible(false);
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/iron_material.json");
    for (int i = 0; i < TASK_PANELS; i++) {
        result->taskIcon[i] = result->AddComponent<mlg::Image>("TaskIcon", material).lock();
        result->taskIcon[i]->SetRelativePosition(result->taskPanel[i]->GetPosition() + glm::vec2(-25.f, -10.f));
        result->taskIcon[i]->SetAnchor({0.0, 1.0});
        result->taskIcon[i]->SetSize({32, 32});
        result->taskIcon[i]->SetVisible(false);
    }

    for (int i = 0; i < TASK_PANELS; i++) {
        result->taskReward[i] = result->AddComponent<mlg::Label>("TaskReward").lock();
        result->taskReward[i]->SetRelativePosition(result->taskPanel[i]->GetPosition() + glm::vec2(22.f, -10.f));
        result->taskReward[i]->SetAnchor({0.0, 1.0});
        result->taskReward[i]->SetSize(20);
        result->taskReward[i]->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
        result->taskReward[i]->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
        result->taskReward[i]->SetVisible(false);
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/iron_material.json");
    for (int i = 0; i < TASK_PANELS; i++) {
        for (int j = 0; j < 2; j++) {
            result->taskRequired[i][j] = result->AddComponent<mlg::Image>("TaskRequired", material).lock();
            result->taskRequired[i][j]->SetAnchor({0.0, 1.0});
            result->taskRequired[i][j]->SetSize({16.f, 16.f});
            result->taskRequired[i][j]->SetVisible(false);
        }
    }

    result->taskManager->GetTaskManager().OnTaskAccepted.append(
            [result](const TaskData& taskData) {
                result->UpdateAllTasks();
            });

    result->taskManager->GetTaskManager().OnTaskFinished.append(
            [result](const TaskData& taskData) {
                result->UpdateAllTasks();
            });

    return result;
}

void GameplayOverlay::Start() {
}

void GameplayOverlay::UpdateAllTasks() {
    for (int i = 0; i < TASK_PANELS; ++i) {
        UpdateTask(i);
    }
}

void GameplayOverlay::UpdateTask(int idx) {
    ProductManager* productManager = ProductManager::Get();
    BlueprintManager* blueprintManager = BlueprintManager::Get();
    std::vector<TaskData> tasks = taskManager->GetTaskManager().GetActiveTasks();

    if (idx >= tasks.size()) {
        SetTaskVisible(idx, false);
        return;
    } else {
        SetTaskVisible(idx, true);
    }

    taskIcon[idx]->material = productManager->GetProduct(tasks[idx].productId).icon;
    const Blueprint& blueprint = blueprintManager->GetBlueprint(tasks[idx].productId);

    for (int j = 0; j < 2; ++j) {
        if (j < blueprint.GetInput().size()) {
            taskRequired[idx][j]->material = productManager->GetProduct(blueprint.GetInput()[j]).icon;
            taskRequired[idx][j]->SetVisible(true);
        } else {
            taskRequired[idx][j]->SetVisible(false);
        }
    }

    if (blueprint.GetInput().size() == 1) {
        taskRequired[idx][0]->SetRelativePosition(taskPanel[idx]->GetPosition() + glm::vec2(0.f, 30.f));
    } else if (blueprint.GetInput().size() == 2) {
        taskRequired[idx][0]->SetRelativePosition(taskPanel[idx]->GetPosition() + glm::vec2(-12.f, 30.f));
        taskRequired[idx][1]->SetRelativePosition(taskPanel[idx]->GetPosition() + glm::vec2(+12.f, 30.f));
    }

    taskReward[idx]->SetText(fmt::format("${:d}", tasks[idx].reward));
    taskBonus[idx]->SetText(fmt::format("${:d}", tasks[idx].bonus));
}

void GameplayOverlay::SetTaskVisible(int idx, bool visible) {
    taskPanel[idx]->SetVisible(visible);
    taskIcon[idx]->SetVisible(visible);
    taskProgress[idx]->SetVisible(visible);
    taskProgressBg[idx]->SetVisible(visible);
    for (int i = 0; i < 2; ++i) {
        taskRequired[idx][i]->SetVisible(visible);
    }
    taskReward[idx]->SetVisible(visible);
    taskPlus[idx]->SetVisible(visible);
    taskBonus[idx]->SetVisible(visible);
}

void GameplayOverlay::Update() {
    auto tasks = taskManager->GetTaskManager().GetActiveTasks();
    int taskCount = taskManager->GetTaskManager().GetActiveTasksCount();
    for (int i = 0; i < taskCount; ++i) {
        float percentage;
        if (tasks[i].timeLimit <= 0.f)
            percentage = 0.f;
        else
            percentage = tasks[i].time / tasks[i].timeLimit;

        taskProgress[i]->percentage = percentage;

        // Hurry up, useless piece of meat!
        if (taskProgress[i]->IsVisible() && percentage <= 0.f) {
            taskProgress[i]->SetVisible(false);
            taskProgressBg[i]->SetVisible(false);
            taskPlus[i]->SetVisible(false);
            taskBonus[i]->SetVisible(false);
        }
    }
}

void GameplayOverlay::SetClock(float time) {
    if (time < 0.f)
        time = 0.f;

    if (time <= 10.f)
        clock->SetTextColor({1.f, 0.f, 0.f});

    int minutes = std::floor(time / 60.f);
    int seconds = std::floor(time - minutes * 60);

    this->clock->SetText(fmt::format("{:02d}:{:02d}", minutes, seconds));
}

void GameplayOverlay::SetScore(int score) {
    this->score->SetText(fmt::format("${:04}", score));
}

void GameplayOverlay::ShowMessage(const std::string& message, float visibleTime) {
    if (mlg::TimerManager::Get()->IsTimerValid(chatTimer))
        mlg::TimerManager::Get()->ClearTimer(chatTimer);

    this->chatWindow->SetVisible(true);

    std::string text = fmt::format("AIPost: {}", message);
    text = mlg::Label::WrapText(text, 42);
    this->chat->SetText(text);

    chatTimer = mlg::TimerManager::Get()->SetTimer(
            visibleTime, false,
            [this]() -> void {
                this->chatWindow->SetVisible(false);
            });
}

bool GameplayOverlay::IsChatVisible() const {
    return this->chatWindow->IsVisible();
}