#include "../../include/UI/GameplayOverlay.h"
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


    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay_background_material.json");
    result->chatWindow = result->AddComponent<mlg::CanvasPanel>("ChatWindow").lock();
    result->chatWindow->SetPosition({640.f, 0.f});
    result->chatWindow->SetSize({400, 200});

    auto ui = result->AddComponent<mlg::Image>("ChatWindow", material).lock();
    ui->SetAnchor({0.5, 0});
    ui->SetSize({400, 200});
    ui->tint = glm::vec4(0.7f, 0.7f, 0.7f, 0.9f);
    result->chatWindow->AddChild(ui);

    ui = result->AddComponent<mlg::Image>("ChatWindow", material).lock();
    ui->SetAnchor({0.5, 0});
    ui->SetSize({400 - 10, 200 - 10});
    ui->tint = glm::vec4(0.f, 0.f, 0.f, 0.95f);
    result->chatWindow->AddChild(ui);

    result->chat = result->AddComponent<mlg::Label>("Chat").lock();
    result->chat->SetRelativePosition({-180.f, 65.f});
    result->chat->SetAnchor({0.5, 1.0});
    result->chat->SetSize(18);
    result->chat->SetText("As a language model, I am unable\nto drive vehicles myself. That is\nwhy you were hired to deliver\npackages.");
    result->chatWindow->AddChild(result->chat);

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/task_panel_material.json");
    for (int i = 0; i < TASK_PANELS; i++) {
        result->taskPanel[i] = result->AddComponent<mlg::Image>("TaskPanel", material).lock();
        result->taskPanel[i]->SetPosition({8 + 48 + i * (96.f + 8.f), 720 - 48});
        result->taskPanel[i]->SetAnchor({0.0, 1.0});
        result->taskPanel[i]->SetSize({96, 96});
        result->taskPanel[i]->SetVisible(false);
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/task_progress_material.json");
    for (int i = 0; i < TASK_PANELS; i++) {
        result->taskProgress[i] = result->AddComponent<mlg::ProgressBar>("TaskProgress", material).lock();
        result->taskProgress[i]->SetPosition(result->taskPanel[i]->GetPosition());
        result->taskProgress[i]->SetAnchor({0.0, 1.0});
        result->taskProgress[i]->SetSize(result->taskPanel[i]->GetSize());
        result->taskProgress[i]->percentage = 0.0f;
        result->taskProgress[i]->SetVisible(false);
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/iron_material.json");
    for (int i = 0; i < TASK_PANELS; i++) {
        result->taskIcon[i] = result->AddComponent<mlg::Image>("TaskIcon", material).lock();
        result->taskIcon[i]->SetRelativePosition(result->taskPanel[i]->GetPosition()
                                         + glm::vec2(0.f, -15.f));
        result->taskIcon[i]->SetAnchor({0.0, 1.0});
        result->taskIcon[i]->SetSize({36, 36});
        result->taskIcon[i]->SetVisible(false);
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

    result->taskManager->GetTaskManager().OnTaskAccepted.append([result](const TaskData& taskData) {
        int count = result->taskManager->GetTaskManager().GetActiveTasksCount();
        auto tasks = result->taskManager->GetTaskManager().GetActiveTasks();

        for (int i = 0; i < count; ++i) {
            result->UpdateTask(i);
        }

        result->taskPanel[count - 1]->SetVisible(true);
        result->taskIcon[count - 1]->SetVisible(true);
        result->taskProgress[count - 1]->SetVisible(true);
        for (int i = 0; i < 2; ++i) {
            result->taskRequired[count - 1][i]->SetVisible(true);
        }
    });

    result->taskManager->GetTaskManager().OnTaskFinished.append([result](const TaskData& taskData) {
        int count = result->taskManager->GetTaskManager().GetActiveTasksCount();
        auto tasks = result->taskManager->GetTaskManager().GetActiveTasks();

        for (int i = 0; i < count; ++i) {
            result->UpdateTask(i);
        }
        result->taskPanel[count - 1]->SetVisible(false);
        result->taskIcon[count - 1]->SetVisible(false);
        result->taskProgress[count - 1]->SetVisible(false);
        for (int i = 0; i < 2; ++i) {
            result->taskRequired[count - 1][i]->SetVisible(false);
        }
    });

    return result;
}

void GameplayOverlay::Start() {
    /*
     * Workaround to display 1st task correctly
     * During level initialization OnTaskAccepted fires before GameplayOverlay is added
     */
    auto tasks = taskManager->GetTaskManager().GetActiveTasks();
    int taskCount = taskManager->GetTaskManager().GetActiveTasksCount();

    for(int i = 0; i < taskCount; ++i) {
        UpdateTask(i);

        taskPanel[i]->SetVisible(true);
        taskIcon[i]->SetVisible(true);
        taskProgress[i]->SetVisible(true);
        for (int j = 0; j < 2; ++j) {
            taskRequired[i][j]->SetVisible(true);
        }
    }
}

void GameplayOverlay::Update() {
    auto tasks = taskManager->GetTaskManager().GetActiveTasks();
    int taskCount = taskManager->GetTaskManager().GetActiveTasksCount();
    for(int i = 0; i < taskCount; ++i) {
        float timeRate = tasks[i].time / tasks[i].timeLimit;
        taskProgress[i]->percentage = timeRate * 0.8f + 0.1f;

        // You useless piece of meat, you are late!
        if (timeRate <= 0.0f) {
            taskPanel[i]->tint = {0.5, 0.0, 0.0, 0.9};
        } else {
            taskPanel[i]->tint = {1.0, 1.0, 1.0, 0.9};
        }
    }
}

void GameplayOverlay::SetScore(int score) {
    this->score->SetText(fmt::format("${:04}", score));
}

void GameplayOverlay::ShowMessage(const std::string& message, float visibleTime) {
    if (mlg::TimerManager::Get()->IsTimerValid(chatTimer))
        mlg::TimerManager::Get()->ClearTimer(chatTimer);

    this->chatWindow->SetVisible(true);

    std::string text = fmt::format("AIPost: {}", message);
    text = mlg::Label::WrapText(text, 40);
    this->chat->SetText(text);

    chatTimer = mlg::TimerManager::Get()->SetTimer(
            visibleTime, false,
            [this]() -> void {
                this->chatWindow->SetVisible(false);
            });
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

void GameplayOverlay::UpdateTask(int idx) {
    auto productManager = ProductManager::Get();
    auto blueprintManager = BlueprintManager::Get();
    auto tasks = taskManager->GetTaskManager().GetActiveTasks();

    taskIcon[idx]->material = productManager->GetProduct(tasks[idx].productId).icon;
    const auto& blueprint = blueprintManager->GetBlueprint(tasks[idx].productId);
    for (int j = 0; j < 2; ++j) {
        if (j < blueprint.GetInput().size()) {
            taskRequired[idx][j]->material = productManager->GetProduct(blueprint.GetInput()[j]).icon;
            taskRequired[idx][j]->SetVisible(true);
        } else {
            taskRequired[idx][j]->SetVisible(false);
        }
    }

    if (blueprint.GetInput().size() == 1) {
        taskRequired[idx][0]->SetRelativePosition(taskPanel[idx]->GetPosition()
                                  + glm::vec2(0.f, 30.f));
    } else if (blueprint.GetInput().size() == 2) {
        taskRequired[idx][0]->SetRelativePosition(taskPanel[idx]->GetPosition()
                      + glm::vec2(-12.f, 30.f));
        taskRequired[idx][1]->SetRelativePosition(taskPanel[idx]->GetPosition()
                                          + glm::vec2(+12.f, 30.f));
    }
    //taskRequiredPanel[idx][0]->SetPosition(taskRequired[idx][0]->GetPosition());
    //taskRequiredPanel[idx][1]->SetPosition(taskRequired[idx][1]->GetPosition());
}
