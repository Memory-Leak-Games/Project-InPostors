#include "Core/Core.h"

#ifdef DEBUG

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#endif

#include "Macros.h"

#include "Core/Time.h"
#include "Core/Window.h"
#include "Core/HID/Input.h"

#include "Rendering/RenderingAPI.h"
#include "Rendering/Renderer.h"
#include "Rendering/CommonUniformBuffer.h"

#include "Rendering/FrameBuffers/GBuffer.h"
#include "Rendering/FrameBuffers/SSAOFrameBuffer.h"
#include "Rendering/FrameBuffers/BlurPass.h"
#include "Rendering/FrameBuffers/PostProcess.h"

#include "Events/WindowEvent.h"

#include "Gameplay/ComponentManager.h"
#include "Gameplay/EntityManager.h"
#include "Rendering/DirectionalLight.h"
#include "Rendering/Gizmos/Gizmos.h"
#include "SceneGraph/SceneGraph.h"
#include "UI/Renderer2D.h"
#include "Physics/Physics.h"

using namespace mlg;

Core* Core::instance;

void Core::MainLoop() {
    DirectionalLight::GetInstance();

    bool shouldClose = false;
    Window::GetInstance()->GetEventDispatcher()->appendListener(
            EventType::WindowClose, [&shouldClose](const Event& event) {
                shouldClose = true;
            });

    ComponentManager::Start();
    EntityManager::Start();

    while (!shouldClose) {
        Time::UpdateStartFrameTime();
        RenderingAPI::GetInstance()->Clear();

#ifdef DEBUG
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif
        ComponentManager::ProcessComponents();
        EntityManager::ProcessEntities();

        Input::Update();

        Physics::TickFixedTimeSteps();

        ComponentManager::Update();
        EntityManager::Update();
        ComponentManager::LateUpdate();
        EntityManager::LateUpdate();

        SceneGraph::CalculateGlobalTransforms();
        CommonUniformBuffer::UpdateAndSendToGPU();

        Renderer::GetInstance()->DrawFrame();

        Gizmos::DrawGizmos();
        Renderer2D::GetInstance()->Draw();

#ifdef DEBUG
        RenderImGUI();
#endif

        Window::GetInstance()->SwapBuffers();
        Window::GetInstance()->PollEvents();

        Time::CapFPS();
        FrameMark;
        TracyGpuCollect;
    }
}

#ifdef DEBUG

void Core::RenderImGUI() const {
    ImGui::Begin("FPS");
    ImGui::Text("Framerate: %.3f (%.1f FPS)", Time::GetTrueDeltaSeconds(), 1 / Time::GetTrueDeltaSeconds());
    ImGui::Text("Time: %.3f", Time::GetSeconds());
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif

void Core::Stop() {
    SPDLOG_INFO("Stopping CoreEngine");

#ifdef DEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    delete Core::instance;
    instance = nullptr;
}

void Core::Initialize() {
    if (Core::instance == nullptr) {
        Core::instance = new Core();
    }

#ifdef DEBUG
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    Window::GetInstance()->ImGuiInit();

    ImGui_ImplOpenGL3_Init("#version 450");

    ImGui::StyleColorsDark();
#endif
}

Core* Core::GetInstance() {
    return Core::instance;
}
