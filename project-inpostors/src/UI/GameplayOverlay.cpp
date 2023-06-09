#include "../../include/UI/GameplayOverlay.h"
#include "SceneGraph/Transform.h"

#include "Core/Time.h"
#include "Core/TimerManager.h"
#include "LevelTaskManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "TaskManager.h"
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

    auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/arialbd.ttf");
    result->clock = result->AddComponent<mlg::Label>("Clock", font).lock();
    result->clock->SetPosition({640.f - 50.f, 720.f - 32.f}); //TODO: Use label's property when I teach it to center text
    result->clock->SetAnchor({0.5, 1.0});
    //result->clock->SetSize(32);

    result->score = result->AddComponent<mlg::Label>("Score", font).lock();
    result->score->SetPosition({1100.f, 720.f - 32.f});
    result->score->SetAnchor({1.0, 1.0});
    result->score->SetText("$0");
    //result->score->SetSize(32);

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/window_material.json");
    auto ui = result->AddComponent<mlg::Image>("ChatWindow", material).lock();
    ui->SetPosition({640.f, 0.f});
    ui->SetAnchor({0.5, 0});
    ui->SetSize({400, 200});

    font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/ocraext.ttf");
    result->chat = result->AddComponent<mlg::Label>("Chat", font).lock();
    result->chat->SetPosition({460.f, 65.f});
    result->chat->SetAnchor({0.5, 1.0});
    result->chat->SetSize(18);
    result->chat->SetText("As a language model, I am unable\nto drive vehicles myself. That is\nwhy you were hired to deliver\npackages.");

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/task_panel_material.json");
    for(int i = 0; i < 10; i++) {
        result->taskPanels[i] = result->AddComponent<mlg::Image>("TaskPanel", material).lock();
        result->taskPanels[i]->SetPosition({8 + 8 + 16 + 48 * i, 720 - 17 - 16});
        result->taskPanels[i]->SetAnchor({0.0, 1.0});
        result->taskPanels[i]->SetSize({48, 48});
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
        auto productManager = ProductManager::GetInstance();
        int idx = result->taskManager->GetTaskManager().GetActiveTasksCount() - 1;

        result->taskIcon[idx]->material = productManager->GetProduct(taskData.productId).icon;

        result->taskPanels[idx]->SetVisible(true);
        result->taskIcon[idx]->SetVisible(true);
        result->taskProgress[idx]->SetVisible(true);
    });

    result->taskManager->GetTaskManager().OnTaskFinished.append([result](const TaskData& taskData) {
        auto productManager = ProductManager::GetInstance();
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
    auto productManager = ProductManager::GetInstance();

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
        taskProgress[i]->percentage = tasks[i].time / tasks[i].timeLimit;
    }
}

void GameplayOverlay::SetScore(int score) {
    this->score->SetText(fmt::format("${}", score));
}

void GameplayOverlay::SetChat(const std::string& message) {
    std::string text = fmt::format("AIPost: {}", message);

    constexpr int chatLimit = 34;
    // wrap text
    for (int i = 0; i < text.size(); i++) {
        if (i % chatLimit == 0 && i != 0) {
            text.insert(i, "\n");
        }
    }

    this->chat->SetText(text);
}

void GameplayOverlay::SetClock(float time) {
    if (time < 0.f)
        time = 0.f;

    int minutes = std::floor(time / 60.f);
    int seconds = std::floor(time - minutes * 60);

    this->clock->SetText(fmt::format("{:02d}:{:02d}", minutes, seconds));
}
