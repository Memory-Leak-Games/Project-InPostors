#include "Core/Core.h"

#ifdef DEBUG

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#endif

#include "Macros.h"

#include "Core/HID/Input.h"
#include "Core/Time.h"
#include "Core/Window.h"

#include "Rendering/CommonUniformBuffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderingAPI.h"

#include "Rendering/FrameBuffers/BlurPass.h"
#include "Rendering/FrameBuffers/GBuffer.h"
#include "Rendering/FrameBuffers/PostProcess.h"
#include "Rendering/FrameBuffers/SSAOFrameBuffer.h"

#include "Events/WindowEvent.h"

#include "Core/Settings/SettingsManager.h"
#include "Gameplay/ComponentManager.h"
#include "Gameplay/EntityManager.h"
#include "Physics/CollisionManager.h"
#include "Physics/Physics.h"
#include "Rendering/DirectionalLight.h"
#include "Rendering/Gizmos/Gizmos.h"
#include "SceneGraph/SceneGraph.h"
#include "UI/Renderer2D.h"

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
        ZoneScoped;

        Time::UpdateStartFrameTime();
        RenderingAPI::GetInstance()->Clear();

#ifdef DEBUG
        {
            ZoneScopedN("ImGui NewFrame");
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
#endif
        TickGameplay();
        SceneGraph::CalculateGlobalTransforms();
        TickRendering();

#ifdef DEBUG
        {
            ZoneScopedN("ImGui Render");
            RenderImGUI();
        }

        if (SettingsManager::Get<bool>(SettingsType::Debug, "showSpacialGrid")) {
            CollisionManager::DrawSpacialGrid();
        }
#endif

        TickWindow();

        Time::CapFPS();
        FrameMark;
        TracyGpuCollect;
    }
}

void Core::TickWindow() const {
    ZoneScopedN("Window Update");
    Window::GetInstance()->SwapBuffers();
    Window::GetInstance()->PollEvents();
}

void Core::TickGameplay() const {
    ZoneScopedN("Tick Gameplay");
    ComponentManager::ProcessComponents();
    EntityManager::ProcessEntities();

    Input::Update();

    Physics::TickFixedTimeSteps();

    ComponentManager::Update();
    EntityManager::Update();
    ComponentManager::LateUpdate();
    EntityManager::LateUpdate();
}

void Core::TickRendering() const {
    ZoneScopedN("Tick Rendering");
    CommonUniformBuffer::UpdateAndSendToGPU();

    Renderer::GetInstance()->DrawFrame();

    Gizmos::DrawGizmos();
    Renderer2D::GetInstance()->Draw();
}

#ifdef DEBUG

void Core::RenderImGUI() const {
    ImGui::Begin("FPS");
    ImGui::Text("Framerate: %.3f (%.1f FPS)", Time::GetTrueDeltaSeconds(), 1 / Time::GetTrueDeltaSeconds());
    ImGui::Text("Time: %.3f", Time::GetSeconds());
    ImGui::End();

    static glm::ivec2 dimensions;
    ImGui::Begin("Spacial Hash Grid");
    ImGui::DragInt2("Dimensions", (int*) &dimensions, 1.f, 1, 100);
    if (ImGui::Button("Update")) {
        CollisionManager::SetBounds(glm::vec2{-50.f}, glm::vec2{50.f}, dimensions);
    }
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
