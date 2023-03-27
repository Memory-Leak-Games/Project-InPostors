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
#include "Rendering/Gizmos/Gizmos.h"
#include "Rendering/Lights.h"

using namespace mlg;

Core* Core::instance;

void Core::MainLoop() {

    // TODO: Remove this
    sceneLight = std::make_shared<Lights>();
    auto begin = std::chrono::high_resolution_clock::now();

    bool shouldClose = false;
    Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowClose, [&shouldClose](const Event& event) {
        shouldClose = true;
    });

    while (!shouldClose) {
        Time::UpdateStartFrameTime();

        RenderingAPI::GetInstance()->Clear();

#ifdef DEBUG
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif

        Input::Update();

        Renderer::GetInstance()->Draw();
        Renderer::GetInstance()->LateDraw();

        // Gizmos test
        {
            Gizmos::DrawLine({-10, 0, 0}, {10, sin(Time::GetSeconds() * 4) * 7, 0}, {0, 1, 0, 1});
            Gizmos::DrawLine({-10, 0, 0}, {10, sin(Time::GetSeconds() * 4 - 1) * 7, 0}, {0, 1, 0, 1}, true);
            Gizmos::DrawBox({1, -3, -1}, {6, 2, 10}, glm::angleAxis(Time::GetSeconds(), glm::vec3(1, 0, 0)), {0, 0, 1, 1});
            Transform transform = Transform();
            transform.SetPosition({-10, 5, -5});
            transform.SetScale(glm::vec3(3, 1, 2) * (sin(Time::GetSeconds()) + 1.1f));
            transform.SetRotation(glm::quat({Time::GetSeconds() * 5, 2, 3}));
            Gizmos::DrawBox(transform, {1, 1, 0, 1});
            Gizmos::DrawSphere({cos(Time::GetSeconds()) * 5, -2, sin(Time::GetSeconds()) * 5}, 10, {1, 0, 1, 1});
            for(int i = 1; i < 10; i++)
                Gizmos::DrawPoint({cos(Time::GetSeconds() + i) * 4, cos(Time::GetSeconds() + i) * sin(Time::GetSeconds() + i) * 4, sin(Time::GetSeconds() + i) * 4}, i, {0, 1, 1, 1}, false);
        }

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
