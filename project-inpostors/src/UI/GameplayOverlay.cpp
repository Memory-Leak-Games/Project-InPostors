#include "../../include/UI/GameplayOverlay.h"
#include "SceneGraph/Transform.h"

#include "Core/Time.h"
#include "Core/TimerManager.h"
#include "Managers/LevelTaskManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Managers/TaskManager.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/ProgressBar.h"
#include "Utils/ProductManager.h"
#include <spdlog/fmt/bundled/format.h>
#include <string>

GameplayOverlay::GameplayOverlay(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent)
    : mlg::Entity(id, name, isStatic, parent) {}

std::shared_ptr<GameplayOverlay> GameplayOverlay::Create(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent,
                                                         std::shared_ptr<class LevelTaskManager> taskManager) {
    auto result = std::shared_ptr<GameplayOverlay>(new GameplayOverlay(id, name, isStatic, parent));

    result->taskManager = taskManager;

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/semi_transparent_background_material.json");
    auto temp = result->AddComponent<mlg::Image>("ClockPanel", material).lock();
    temp->SetSize({110, 48});
    temp->SetPosition({640, 720 - 8 - 24});
    temp->SetAnchor({0.5, 1.0});

    auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/terminus-bold.ttf");
    result->clock = result->AddComponent<mlg::Label>("Clock").lock();
    result->clock->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
    result->clock->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
    //result->clock->SetPosition({640.f - 42.f, 720.f - 32.f - 2 - 8.f}); //TODO: Use label's property when I teach it to center text
    result->clock->SetPosition({640.f, 720.f - 24 - 2 - 8.f});
    result->clock->SetAnchor({0.5, 1.0});

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/semi_transparent_background_material.json");
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

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/semi_transparent_background_material.json");
    auto ui = result->AddComponent<mlg::Image>("ChatWindow", material).lock();
    ui->SetPosition({640.f, 0.f});
    ui->SetAnchor({0.5, 0});
    ui->SetSize({400, 200});
    ui->tint = glm::vec4(0.7f, 0.7f, 0.7f, 0.9f);

    ui = result->AddComponent<mlg::Image>("ChatWindow", material).lock();
    ui->SetPosition({640.f, 0.f});
    ui->SetAnchor({0.5, 0});
    ui->SetSize({400 - 10, 200 - 10});
    ui->tint = glm::vec4(0.f, 0.f, 0.f, 0.95f);

    font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/terminus-bold.ttf");
    result->chat = result->AddComponent<mlg::Label>("Chat").lock();
    result->chat->SetPosition({460.f, 65.f});
    result->chat->SetAnchor({0.5, 1.0});
    result->chat->SetSize(18);
    result->chat->SetText("As a language model, I am unable\nto drive vehicles myself. That is\nwhy you were hired to deliver\npackages.");

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/task_panel_material.json");
    for(int i = 0; i < 10; i++) {
        result->taskPanels[i] = result->AddComponent<mlg::Image>("TaskPanel", material).lock();
        result->taskPanels[i]->SetPosition({8 + 8 + 16 + 48 * i, 720 - 17 - 16});
        result->taskPanels[i]->SetAnchor({0.0, 1.0});
        result->taskPanels[i]->SetSize({42, 42});
        result->taskPanels[i]->SetVisible(false);
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/iron_material.json");
    for(int i = 0; i < 10; i++) {
        result->taskIcon[i] = result->AddComponent<mlg::Image>("TaskIcon", material).lock();
        result->taskIcon[i]->SetPosition({8 + 8 + 16 + 48 * i, 720 - 17 - 16});
        result->taskIcon[i]->SetAnchor({0.0, 1.0});
        result->taskIcon[i]->SetSize({24, 24});
        result->taskIcon[i]->SetVisible(false);
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/task_progress_material.json");
    for(int i = 0; i < 10; i++)
    {
        result->taskProgress[i] = result->AddComponent<mlg::ProgressBar>("TaskProgress", material).lock();
        result->taskProgress[i]->SetPosition({8 + 8 + 16 + 48 * i, 720 - 17 - 16});
        result->taskProgress[i]->SetAnchor({0.0, 1.0});
        result->taskProgress[i]->SetSize({64, 64});
        result->taskProgress[i]->percentage = 1.0f;
        result->taskProgress[i]->SetVisible(false);
    }

    result->taskManager->GetTaskManager().OnTaskAccepted.append([result](const TaskData& taskData) {
//        auto productManager = ProductManager::GetInstance();
//        int idx = result->taskManager->GetTaskManager().GetActiveTasksCount() - 1;
//
//        result->taskIcon[idx]->material = productManager->GetProduct(taskData.productId).icon;

        auto productManager = ProductManager::Get();
        int count = result->taskManager->GetTaskManager().GetActiveTasksCount();
        auto tasks = result->taskManager->GetTaskManager().GetActiveTasks();

        for(int i = 0; i < count; i++) {
            result->taskIcon[i]->material = productManager->GetProduct(tasks[i].productId).icon;
        }

        result->taskPanels[count - 1]->SetVisible(true);
        result->taskIcon[count - 1]->SetVisible(true);
        result->taskProgress[count - 1]->SetVisible(true);
    });

    result->taskManager->GetTaskManager().OnTaskFinished.append([result](const TaskData& taskData) {
        auto productManager = ProductManager::Get();
        int count = result->taskManager->GetTaskManager().GetActiveTasksCount();
        auto tasks = result->taskManager->GetTaskManager().GetActiveTasks();

        for(int i = 0; i < count - 1; i++) {
            result->taskIcon[i]->material = productManager->GetProduct(tasks[i].productId).icon;
        }
        result->taskPanels[count - 1]->SetVisible(false);
        result->taskIcon[count - 1]->SetVisible(false);
        result->taskProgress[count - 1]->SetVisible(false);
    });

    return result;
}

void GameplayOverlay::Start() {
    timer = mlg::TimerManager::Get()->SetTimer(180, false, []() -> void {
    });

    /*
     * Workaround to display 1st task correctly
     * During level initialization OnTaskAccepted fires before GameplayOverlay is added
     */
    auto tasks = taskManager->GetTaskManager().GetActiveTasks();
    int taskCount = taskManager->GetTaskManager().GetActiveTasksCount();
    auto productManager = ProductManager::Get();

    for(int i = 0; i < taskCount; i++) {
        taskIcon[i]->material = productManager->GetProduct(tasks[i].productId).icon;
        taskPanels[i]->SetVisible(true);
        taskIcon[i]->SetVisible(true);
        taskProgress[i]->SetVisible(true);
    }
}

void GameplayOverlay::Update() {
    auto tasks = taskManager->GetTaskManager().GetActiveTasks();
    int taskCount = taskManager->GetTaskManager().GetActiveTasksCount();
    for(int i = 0; i < taskCount; i++) {
        float timeRate = tasks[i].time / tasks[i].timeLimit;
        taskProgress[i]->percentage = timeRate;

        // You useless piece of meat, you are late!
        if(timeRate <= 0.0f) {
            taskPanels[i]->tint = {0.5, 0.0, 0.0, 0.85};
            taskProgress[i]->SetVisible(false);
        }
        else {
            taskPanels[i]->tint = {0.0, 0.0, 0.0, 0.85};
            taskProgress[i]->SetVisible(true);
        }
    }
}

void GameplayOverlay::SetScore(int score) {
    this->score->SetText(fmt::format("${:04}", score));
}

void GameplayOverlay::SetChat(const std::string& message) {
    std::string text = fmt::format("AIPost: {}", message);

    constexpr int chatLimit = 40;
    std::string wrappedText = mlg::Label::WrapText(text, 40);
    this->chat->SetText(wrappedText);
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
