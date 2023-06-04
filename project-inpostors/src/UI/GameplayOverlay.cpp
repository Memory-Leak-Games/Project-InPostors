#include "../../include/UI/GameplayOverlay.h"
#include "SceneGraph/Transform.h"

#include "Core/Time.h"
#include "Core/TimerManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/Components/ProgressBar.h"
#include <spdlog/fmt/bundled/format.h>
#include <string>

GameplayOverlay::GameplayOverlay(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent)
    : mlg::Entity(id, name, isStatic, parent) {}

std::shared_ptr<GameplayOverlay> GameplayOverlay::Create(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent) {
    auto result = std::shared_ptr<GameplayOverlay>(new GameplayOverlay(id, name, isStatic, parent));

    auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/arialbd.ttf");
    result->clock = result->AddComponent<mlg::Label>("Clock", font).lock();
    result->clock->SetPosition({640.f - 50.f, 720.f - 32.f}); //TODO: Use label's property when I teach it to center text
    result->clock->SetAnchor({0.5, 1.0});
    //result->clock->SetSize(32);

    result->score = result->AddComponent<mlg::Label>("Score", font).lock();
    result->score->SetPosition({1100.f, 720.f - 32.f});
    result->score->SetAnchor({1.0, 1.0});
    result->score->SetText("Score: $0");
    //result->score->SetSize(32);

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/gameplay/window_material.json");
    auto ui = result->AddComponent<mlg::Image>("ChatWindow", material).lock();
    ui->SetPosition({640.f, 0.f});
    ui->SetAnchor({0.5, 0});
    ui->SetSize({400, 200});

    font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/OCRAEXT.TTF");
    result->chat = result->AddComponent<mlg::Label>("Chat", font).lock();
    result->chat->SetPosition({460.f, 65.f});
    result->chat->SetAnchor({0.5, 1.0});
    result->chat->SetSize(18);
    result->chat->SetText("As a language model, I am unable\nto drive vehicles myself. That is\nwhy you were hired to deliver\npackages.");

    // THIS IS VERY MUCH MOCK
    //    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/player/panel_material.json");
    //    ui = result->AddComponent<mlg::Image>("QuestPanel", material).lock();
    //    ui->SetPosition({85.f, 720 - 50.f});
    //    ui->SetAnchor({0, 1});
    //    ui->SetSize({150, 100});
    //
    //    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/furniture_material.json");
    //    ui = result->AddComponent<mlg::Image>("QuestItem", material).lock();
    //    ui->SetPosition({85.f, 720.f - 30.f});
    //    ui->SetAnchor({0, 1});
    //    ui->SetSize({32, 32});
    //
    //    font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/arialbd.ttf");
    //    auto label = result->AddComponent<mlg::Label>("QuestLabel", font).lock();
    //    label->SetPosition({40.f, 720.f - 75.f});
    //    label->SetAnchor({0.0, 1.0});
    //    label->SetSize(20);
    //    label->SetText("Furniture");
    //    label->SetTextColor({0, 0, 0});
    //
    //    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/factory/progress_bar_material.json");
    //    result->questBar = result->AddComponent<mlg::ProgressBar>("QuestLimit", material).lock();
    //    result->questBar->SetPosition({85.f, 720.f - 95.f});
    //    result->questBar->SetAnchor({0.0, 1.0});
    //    result->questBar->SetSize({150, 5});

    return result;
}

void GameplayOverlay::Start() {
    timer = mlg::TimerManager::Get()->SetTimer(180, false, []() -> void {
    });
}

void GameplayOverlay::Update() { }

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
