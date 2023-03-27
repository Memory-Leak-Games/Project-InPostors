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
#include "Rendering/PostProcess.h"
#include "Rendering/Camera.h"
#include "Rendering/GBuffer.h"

using namespace mlg;

Core* Core::instance;

void Core::MainLoop() {

    // TODO: Remove this
    sceneLight = std::make_shared<Lights>();
    auto begin = std::chrono::high_resolution_clock::now();

    PostProcess postProcessingFrameBuffer(Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight());
    GBuffer gBuffer(Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight());

    Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize, [&postProcessingFrameBuffer, &gBuffer](const Event& event) {
        auto& windowResizeEvent = (WindowResizeEvent&) event;
        RenderingAPI::GetInstance()->SetViewport(0, 0, windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());

        postProcessingFrameBuffer.Resize(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
        gBuffer.Resize(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());

        Camera::GetInstance()->SetResolution({windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight()});
    });

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

        gBuffer.Activate();

        Renderer::GetInstance()->Draw();

        gBuffer.DeActivate();
        gBuffer.CopyDepthBuffer();

        Renderer::GetInstance()->LateDraw();

        postProcessingFrameBuffer.Activate();
        postProcessingFrameBuffer.Clear({0.f, 0.f, 0.f, 1.f});

        gBuffer.Draw();

        postProcessingFrameBuffer.DeActivate();
//        postProcessingFrameBuffer.Draw();

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

        ImGui::Separator();
        bool vSync = Window::GetInstance()->GetVerticalSync();
        ImGui::Checkbox("VSync ", &vSync);
        Window::GetInstance()->SetVerticalSync(vSync);

        ImGui::Separator();
        ImGui::Text("Camera");
        glm::vec3 position = Camera::GetInstance()->GetPosition();
        ImGui::DragFloat3("Camera Position", (float*) &position);
        Camera::GetInstance()->SetPosition(position);

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
