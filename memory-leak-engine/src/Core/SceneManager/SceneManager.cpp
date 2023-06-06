#include "Core/SceneManager/SceneManager.h"

#include "Core/Core.h"
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
    auto loadScene = [&scene]() {
        if (instance->currentScene != nullptr)
            instance->currentScene->UnLoad();
        
        Initializer::StopSceneComponents();
        Initializer::InitializeSceneComponents();
        
        instance->currentScene = std::move(scene);
        instance->currentScene->Load();
    };

    if (mlg::Core::GetInstance()->IsClosed()) {
        loadScene();
        return;
    }

    mlg::Core::GetInstance()->OnMainLoopEnd.append(loadScene);
    mlg::Core::GetInstance()->Close();
}

void mlg::SceneManager::Update() {
    if (instance->currentScene == nullptr)
        return;

    instance->currentScene->Update();
}

void mlg::SceneManager::ExitGame() {
    mlg::Core::GetInstance()->Close();
}

mlg::Scene* mlg::SceneManager::GetCurrentScene() {
    return instance->currentScene.get();
}
