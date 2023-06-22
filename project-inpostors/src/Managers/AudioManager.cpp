#include "Managers/AudioManager.h"

#include "Audio/Assets/AudioAsset.h"
#include "Core/AssetManager/AssetManager.h"

#include "Managers/TaskManager.h"

#include "Core/SceneManager/SceneManager.h"
#include "Scenes/LevelScene.h"

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
    audioManager->mediumFinishTaskSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/sfx/coins.mp3");
    audioManager->superFinishTaskSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/sfx/tada.mp3");


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
            [this](const TaskData& taskData) {
                int price = taskData.reward;
                if (taskData.time > 0.f)
                    price += taskData.bonus;

                finishTaskSound->Play();
                if (price > 100.f)
                    mediumFinishTaskSound->Play();

                if (price > 200.f)
                    superFinishTaskSound->Play();
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
