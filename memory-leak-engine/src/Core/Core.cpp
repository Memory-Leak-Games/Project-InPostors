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
#include "Rendering/CommonUniformBuffer.h"

#include "Rendering/FrameBuffers/FrameBuffer.h"
#include "Rendering/FrameBuffers/GBuffer.h"
#include "Rendering/FrameBuffers/SSAO.h"
#include "Rendering/FrameBuffers/BlurPass.h"
#include "Rendering/FrameBuffers/PostProcess.h"

#include "Events/WindowEvent.h"

// TODO: delete this
#include "Rendering/Gizmos/Gizmos.h"
#include "Rendering/DirectionalLight.h"
#include "Rendering/Camera.h"
#include "Gameplay/ComponentManager.h"
#include "Gameplay/EntityManager.h"
#include "SceneGraph/SceneGraph.h"
#include "Rendering/Camera.h"

using namespace mlg;

Core* Core::instance;

void Core::MainLoop() {

    // TODO: Remove this
    sceneLight = std::make_shared<DirectionalLight>();

    int32_t windowWidth = Window::GetInstance()->GetWidth();
    int32_t windowHeight = Window::GetInstance()->GetHeight();

    GBuffer gBuffer(windowWidth, windowHeight);
    SSAO ssao(windowWidth, windowHeight);
    BlurPass blurPass(windowWidth, windowHeight);
    PostProcess postProcessingFrameBuffer(windowWidth, windowHeight);

    Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize,
                                                                [&postProcessingFrameBuffer, &gBuffer, &ssao](
                                                                        const Event& event) {
                                                                    auto& windowResizeEvent = (WindowResizeEvent&) event;
                                                                    RenderingAPI::GetInstance()->SetViewport(0, 0,
                                                                                                             windowResizeEvent.GetWidth(),
                                                                                                             windowResizeEvent.GetHeight());

                                                                    int32_t windowWidth = windowResizeEvent.GetWidth();
                                                                    int32_t windowHeight = windowResizeEvent.GetHeight();

                                                                    gBuffer.Resize(windowWidth, windowHeight);
                                                                    ssao.Resize(windowWidth, windowHeight);
                                                                    postProcessingFrameBuffer.Resize(windowWidth,
                                                                                                     windowHeight);

                                                                    Camera::GetInstance()->SetResolution(
                                                                            {windowResizeEvent.GetWidth(),
                                                                             windowResizeEvent.GetHeight()});
                                                                });

    bool shouldClose = false;
    Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowClose,
                                                                [&shouldClose](const Event& event) {
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
        CommonUniformBuffer::UpdateAndSendToGPU();

        gBuffer.Activate();
        gBuffer.Clear();
        Renderer::GetInstance()->Draw(&gBuffer);
        gBuffer.CopyDepthBuffer(postProcessingFrameBuffer.GetFbo());

        ssao.BindTextureUnits(gBuffer.GetPositionTexture(), gBuffer.GetNormalTexture());
        ssao.Draw();
        blurPass.BindTextureUnits(ssao.GetOutput());
        blurPass.Draw();

        postProcessingFrameBuffer.Activate();

        gBuffer.BindTextures(blurPass.GetBlurredTexture());
        gBuffer.Draw();
        Renderer::GetInstance()->LateDraw(&postProcessingFrameBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        postProcessingFrameBuffer.Draw();
        postProcessingFrameBuffer.CopyDepthBuffer(0);

        Gizmos::DrawGizmos();

#ifdef DEBUG
        ImGui::Begin("FPS");
        ImGui::Text("Framerate: %.3f (%.1f FPS)", Time::GetTrueDeltaSeconds(), 1 / Time::GetTrueDeltaSeconds());
        ImGui::Text("Time: %.3f", Time::GetSeconds());

        ImGui::Separator();

        for (const std::string& action : {"test_button", "test_axis"}) {
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
