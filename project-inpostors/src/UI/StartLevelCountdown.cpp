#include "UI/StartLevelCountdown.h"

#include "Core/HID/Input.h"
#include "Core/TimerManager.h"
#include "UI/Components/Label.h"

StartLevelCountdown::~StartLevelCountdown() = default;

StartLevelCountdown::StartLevelCountdown(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {}

std::shared_ptr<StartLevelCountdown> StartLevelCountdown::Create(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent) {
    auto startLevelCountdown =
            std::shared_ptr<StartLevelCountdown>(
                    new StartLevelCountdown(id, name, isStatic, parent));
    startLevelCountdown->InitializeCountdownLabel();

    return startLevelCountdown;
}

void StartLevelCountdown::Start() {
    if (timeToStart < 0.f)
        return;

    mlg::Time::PauseGame(true);
    startTime = mlg::Time::GetTrueSeconds();
}

void StartLevelCountdown::Update() {
    float timeFromStart = mlg::Time::GetTrueSeconds() - startTime;
    countdownLabel.lock()->SetText(
            fmt::format("{}", std::ceil(timeToStart - timeFromStart)));

    if (timeFromStart > timeToStart) {
        mlg::Time::PauseGame(false);
        countdownLabel.lock()->SetText("GO!");
    }

    if (timeFromStart > timeToStart + 1.f) {
        mlg::Time::PauseGame(false);
        QueueForDeletion();
    }

    if (mlg::Input::IsActionJustPressed("pause")) {
        mlg::Time::PauseGame(false);
        QueueForDeletion();
    }
}

void StartLevelCountdown::SetTimeToStart(int timeToStart) {
    this->timeToStart = timeToStart;
}

void StartLevelCountdown::InitializeCountdownLabel() {
    countdownLabel = AddComponent<mlg::Label>("CountdownLabel");
    auto sharedCountdownLabel = countdownLabel.lock();
    sharedCountdownLabel->SetPosition(MLG_POS_CENTER);
    sharedCountdownLabel->SetAnchor(MLG_ANCHOR_CENTER);
    sharedCountdownLabel->SetSize(100.f);
    sharedCountdownLabel->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
    sharedCountdownLabel->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
}