#include "Managers/AudioManager.h"

#include "Audio/Assets/AudioAsset.h"
#include "Core/AssetManager/AssetManager.h"

#include "Managers/TaskManager.h"

#include "Core/SceneManager/SceneManager.h"
#include "Scenes/LevelScene.h"
#include <cmath>
#include <spdlog/spdlog.h>

AudioManager::AudioManager(uint64_t id,
                           const std::string& name,
                           bool isStatic,
                           mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {}

AudioManager::~AudioManager() = default;

std::shared_ptr<AudioManager> AudioManager::Create(
        uint64_t id,
        const std::string& name,
        bool isStatic,
        mlg::Transform* parent) {
    auto audioManager = std::shared_ptr<AudioManager>(
            new AudioManager(id, name, isStatic, parent));

    audioManager->finishTaskSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/sfx/cha-ching.wav");
    audioManager->startLevelCountdownSound =
            mlg::AssetManager::GetAsset<mlg::AudioAsset>(
                    "res/audio/sfx/start_level_countdown.wav");
    audioManager->finishCountdownSound =
            mlg::AssetManager::GetAsset<mlg::AudioAsset>(
                    "res/audio/sfx/go_level_countdown.wav");

    audioManager->cityAmbientSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/music/city_ambient.mp3");

    audioManager->clockCountdownSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/sfx/clock_countdown.wav");

    audioManager->boxingBellSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/sfx/boxing_bell.mp3");

    return audioManager;
}

void AudioManager::Start() {
    mlg::Scene* scene = mlg::SceneManager::GetCurrentScene();
    LevelScene* levelScene = dynamic_cast<LevelScene*>(scene);

    levelScene->GetTaskManager()->OnTaskFinished.append(
            [this](const auto& TaskData) {
                finishTaskSound->Play();
            });
    levelScene->GetLevelCountdown().OnCountdownTick.append(
            [this]() {
                startLevelCountdownSound->Play();
            });
    levelScene->GetLevelCountdown().OnCountdownFinished.append(
            [this]() {
                finishCountdownSound->Play();
            });

    levelScene->OnLevelFinished.append(
            [this]() {
                boxingBellSound->Play();
            });

    cityAmbientSound->Play();
}

void AudioManager::Update() {
}

void AudioManager::SetTimeLeft(float timeLeft) {
    int wholeSecondLeft = std::ceil(timeLeft);

    if (lastWholeSecondLeft == -1)
        lastWholeSecondLeft = wholeSecondLeft;

    if (wholeSecondLeft <= 11 &&
        wholeSecondLeft > 0 &&
        lastWholeSecondLeft != wholeSecondLeft) {
        clockCountdownSound->Play();
    }

    lastWholeSecondLeft = wholeSecondLeft;
}
