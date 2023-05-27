#include "Initializer.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/Core.h"
#include "Core/Settings/SettingsManager.h"
#include "Core/Time.h"
#include "Core/TimerManager.h"
#include "Core/Window.h"
#include "Core/HID/Input.h"

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
    UIController::Initialize();
}

void mlg::Initializer::StopCoreComponents() {
    UIController::Stop();
    Input::Stop();
    Core::Stop();

    AudioAPI::Stop();

    Window::Stop();
    AssetManager::Stop();

    Time::Stop();
    SettingsManager::Stop();
}

void mlg::Initializer::InitializeSceneComponents() {
    RenderingAPI::Initialize();
    Renderer::Initialize();
    UIRenderer::Initialize();
    CommonUniformBuffer::Initialize();

    SceneGraph::Initialize();
    Gizmos::Initialize();

    TimerManager::Initialize();
    Physics::Initialize();

    ComponentManager::Initialize();
    EntityManager::Initialize();
}

void mlg::Initializer::StopSceneComponents() {
    EntityManager::Stop();
    ComponentManager::Stop();

    Physics::Stop();
    TimerManager::Stop();

    Gizmos::Stop();
    SceneGraph::Stop();
    
    CommonUniformBuffer::Stop();
    UIRenderer::Stop();
    Renderer::Stop();
    RenderingAPI::Stop();
}
