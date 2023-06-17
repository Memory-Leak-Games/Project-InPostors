#include "Initializer.h"

#include "Core/Core.h"
#include "Core/AssetManager/AssetManager.h"
#include "Core/SceneManager/SceneManager.h"
#include "Core/Settings/SettingsManager.h"
#include "Core/Time.h"
#include "Core/TimerManager.h"
#include "Core/Window.h"
#include "Core/HID/Input.h"

#include "Rendering/DirectionalLight.h"
#include "Rendering/RenderingAPI.h"
#include "Rendering/Renderer.h"
#include "Rendering/Gizmos/Gizmos.h"
#include "Rendering/CommonUniformBuffer.h"

#include "SceneGraph/SceneGraph.h"

#include "Gameplay/ComponentManager.h"
#include "Gameplay/EntityManager.h"

#include "UI/UIController.h"
#include "UI/UIRenderer.h"

#include "Audio/AudioAPI.h"

#include "Physics/Physics.h"


void mlg::Initializer::InitializeCoreComponents() {
    SettingsManager::Initialize();
    Time::Initialize();

    AssetManager::Initialize();
    Window::Initialize("Memory Leak Engine");
    AudioAPI::Initialize();

    Core::Initialize();
    Input::Initialize();

    RenderingAPI::Initialize();
    Gizmos::Initialize();

    SceneManager::Initialize();
}

void mlg::Initializer::StopCoreComponents() {
    SceneManager::Stop();

    Gizmos::Stop();
    RenderingAPI::Stop();

    Input::Stop();
    Core::Stop();

    AudioAPI::Stop();

    Window::Stop();
    AssetManager::Stop();

    Time::Stop();
    SettingsManager::Stop();
}

void mlg::Initializer::InitializeSceneComponents() {
    UIController::Initialize();
    Renderer::Initialize();
    UIRenderer::Initialize();
    CommonUniformBuffer::Initialize();

    SceneGraph::Initialize();

    TimerManager::Initialize();
    Physics::Initialize();

    ComponentManager::Initialize();
    EntityManager::Initialize();
}

void mlg::Initializer::StopSceneComponents() {
    EntityManager::Stop();
    ComponentManager::Stop();

    DirectionalLight::Stop();

    Physics::Stop();
    TimerManager::Stop();

    SceneGraph::Stop();
    
    CommonUniformBuffer::Stop();
    UIRenderer::Stop();
    Renderer::Stop();
    UIController::Stop();
}
