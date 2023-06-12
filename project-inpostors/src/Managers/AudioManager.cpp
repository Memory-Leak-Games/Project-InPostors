#include "Managers/AudioManager.h"

#include "Audio/Assets/AudioAsset.h"
#include "Core/AssetManager/AssetManager.h"

#include "Managers/TaskManager.h"

#include "Core/SceneManager/SceneManager.h"
#include "Scenes/LevelScene.h"
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
            "res/audio/sfx/finish_task.mp3");

    return audioManager;
}

void AudioManager::Start() {
    mlg::Scene* scene = mlg::SceneManager::GetCurrentScene();
    LevelScene* levelScene = dynamic_cast<LevelScene*>(scene);

    levelScene->GetTaskManager()->OnTaskFinished.append(
            [this](const auto& TaskData) {
                finishTaskSound->Play();
            });
}

void AudioManager::Update() {
}
