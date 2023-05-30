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

    instance->actualScene->UnLoad();

    delete instance;
    instance = nullptr;
}

void mlg::SceneManager::LoadScene(std::unique_ptr<Scene> scene) {
    if (instance->actualScene != nullptr)
        instance->actualScene->UnLoad();
    
    Initializer::StopSceneComponents();
    Initializer::InitializeSceneComponents();
    
    instance->actualScene = std::move(scene);
    instance->actualScene->Load();
}

void mlg::SceneManager::Update() {
    instance->actualScene->Update();
}
