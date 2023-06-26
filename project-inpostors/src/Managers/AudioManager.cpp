#include "Managers/AudioManager.h"
#include <Core/TimerManager.h>

#include "Audio/Assets/AudioAsset.h"
#include "Core/AssetManager/AssetManager.h"

#include "Managers/TaskManager.h"

#include "Core/SceneManager/SceneManager.h"
#include "Scenes/LevelScene.h"

using Random = effolkronium::random_static;

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

    audioManager->carHorn1 = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/random_sounds/carhorn1.mp3");

    audioManager->carHorn2 = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/random_sounds/carhorn2.mp3");

    audioManager->carHorn3 = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/random_sounds/carhorn3.mp3");

    audioManager->carHorn4 = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/random_sounds/carhorn4.wav");

    audioManager->seagull = mlg::AssetManager::GetAsset<mlg::AudioAsset>(
            "res/audio/random_sounds/seagull.mp3");


    return audioManager;
}

void AudioManager::Start() {
    mlg::Scene* scene = mlg::SceneManager::GetCurrentScene();
    LevelScene* levelScene = dynamic_cast<LevelScene*>(scene);

    randomSounds = {carHorn1, carHorn2, carHorn3, carHorn4, seagull};
    cityAmbientSound->Play();

    StartRandomSoundTimer();
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
}

void AudioManager::Update() {
}

void AudioManager::StartRandomSoundTimer() {
    float delay = Random::get(
            minDelayBetweenRandomSounds,
            maxDelayBetweenRandomSounds);

    if (mlg::TimerManager::Get()->IsTimerValid(randomSoundTimer)) {
        mlg::TimerManager::Get()->ClearTimer(randomSoundTimer);
    }

    int index = Random::get(0, int(randomSounds.size()-1));

    while (index == currentSoundIndex)
    {
        index = Random::get(0, int(randomSounds.size()-1));
    }

    currentSoundIndex = index;

    randomSoundTimer = mlg::TimerManager::Get()->SetTimer(
            delay,
            false,
            [this, index](){
                randomSounds[index]->Play();
                StartRandomSoundTimer();
            });
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
