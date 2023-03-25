#include "Core/Core.h"

#ifdef DEBUG

#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#endif

#include "Macros.h"

#include "Core/Time.h"
#include "Core/Window.h"
#include "Core/HID/Input.h"

#include "Rendering/RenderingAPI.h"
#include "Rendering/Renderer.h"

#include "Events/WindowEvent.h"

// TODO: delete this
#include "Rendering/Lights.h"
#include "Gameplay/ComponentManager.h"
#include "Gameplay/EntityManager.h"
#include "SceneGraph/SceneGraph.h"
#include "Rendering/FrameBuffer.h"

using namespace mlg;

Core* Core::instance;

void Core::MainLoop() {

    // TODO: Remove this
    sceneLight = std::make_shared<Lights>();
    auto begin = std::chrono::high_resolution_clock::now();

    FrameBuffer postProcessingFrameBuffer(Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight());

    bool shouldClose = false;
    Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowClose, [&shouldClose](const Event& event) {
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

        ComponentManager::Update();
        EntityManager::Update();

        ComponentManager::LateUpdate();
        EntityManager::LateUpdate();

        SceneGraph::CalculateGlobalTransforms();

        postProcessingFrameBuffer.Activate();
        postProcessingFrameBuffer.Clear({0.f, 0.f, 0.f, 1.f});

        Renderer::GetInstance()->Draw();
        Renderer::GetInstance()->LateDraw();

        postProcessingFrameBuffer.DeActivate();
        postProcessingFrameBuffer.Draw();

#ifdef DEBUG
        ImGui::Begin("FPS");
        ImGui::Text("Framerate: %.3f (%.1f FPS)", Time::GetTrueDeltaSeconds(), 1 / Time::GetTrueDeltaSeconds());
        ImGui::Text("Time: %.3f", Time::GetSeconds());

        ImGui::Separator();

        for (const std::string& action : {"test_button", "test_axis"})
        {
            float testFloat = Input::GetActionStrength(action);
            bool isTestPressed = Input::IsActionPressed(action);
            bool isTestJustPressed = Input::IsActionJustPressed(action);
            bool isTestJustReleased = Input::IsActionJustReleased(action);

            ImGui::Text("%s", action.c_str());
            ImGui::Text("Strength: %f", testFloat);
            ImGui::Text("State: %b, JustPressed: %b, JustReleased: %b", isTestPressed,
                        isTestJustPressed, isTestJustReleased);
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        Window::GetInstance()->SwapBuffers();
        Window::GetInstance()->PollEvents();
    }

}

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

    ImGui_ImplOpenGL3_Init("#version 460");

    ImGui::StyleColorsDark();
#endif
}

Core* Core::GetInstance() {
    return Core::instance;
}
