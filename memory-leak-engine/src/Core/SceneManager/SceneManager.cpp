#include "Core/SceneManager/SceneManager.h"

#include "Core/SceneManager/Scene.h"
#include "Initializer.h"

mlg::SceneManager* mlg::SceneManager::instance = nullptr;

mlg::SceneManager::~SceneManager() = default;

void mlg::SceneManager::Initialize() {
    if (instance != nullptr)
        return;

    SPDLOG_INFO("Initializing SceneManager");
    instance = new SceneManager();
}

void mlg::SceneManager::Stop() {
    if (instance == nullptr)
        return;

    SPDLOG_DEBUG("Stopping SceneManager");

    instance->currentScene->UnLoad();

    delete instance;
    instance = nullptr;
}

void mlg::SceneManager::LoadScene(std::unique_ptr<Scene> scene) {
    if (instance->currentScene != nullptr)
        instance->currentScene->UnLoad();
    
    Initializer::StopSceneComponents();
    Initializer::InitializeSceneComponents();
    
    instance->currentScene = std::move(scene);
    instance->currentScene->Load();
}

void mlg::SceneManager::Update() {
    instance->currentScene->Update();
}

mlg::Scene* mlg::SceneManager::GetCurrentScene() {
    return instance->currentScene.get();
}
