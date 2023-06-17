#include "Core/SceneManager/SceneManager.h"

#include "Core/Core.h"
#include "Core/SceneManager/LoadingScreen.h"
#include "Core/SceneManager/Scene.h"
#include "Initializer.h"

mlg::SceneManager* mlg::SceneManager::instance = nullptr;

mlg::SceneManager::SceneManager()
    : loadingScreen(std::make_unique<LoadingScreen>()),
      nextScene(nullptr),
      currentScene(nullptr) {}

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

void mlg::SceneManager::SetNextScene(std::unique_ptr<Scene> scene) {
    instance->nextScene = std::move(scene);

    if (mlg::Core::Get()->IsClosed()) {
        return;
    }

    mlg::Core::Get()->Close();
}

void mlg::SceneManager::LoadNextScene() {

    if (instance->currentScene != nullptr)
        instance->currentScene->UnLoad();

    Initializer::StopSceneComponents();
    Initializer::InitializeSceneComponents();

    instance->loadingScreen->Draw();

    instance->currentScene = std::move(instance->nextScene);
    instance->currentScene->Load();
    mlg::Core::Get()->MainLoop();
}

bool mlg::SceneManager::HasNextScene() {
    return instance->nextScene != nullptr;
}

void mlg::SceneManager::Update() {
    if (instance->currentScene == nullptr)
        return;

    instance->currentScene->Update();
}

void mlg::SceneManager::ExitGame() {
    mlg::Core::Get()->Close();
}

mlg::Scene* mlg::SceneManager::GetCurrentScene() {
    return instance->currentScene.get();
}
