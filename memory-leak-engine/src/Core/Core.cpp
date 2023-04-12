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

#include "Rendering/FrameBuffers/GBuffer.h"
#include "Rendering/FrameBuffers/SSAO.h"
#include "Rendering/FrameBuffers/BlurPass.h"
#include "Rendering/FrameBuffers/PostProcess.h"

#include "Events/WindowEvent.h"

#include "Gameplay/ComponentManager.h"
#include "Gameplay/EntityManager.h"
#include "Rendering/Camera.h"
#include "Rendering/DirectionalLight.h"
#include "Rendering/Gizmos/Gizmos.h"
#include "SceneGraph/SceneGraph.h"
#include "UI/Renderer2D.h"
#include "Physics/Physics.h"

using namespace mlg;

Core* Core::instance;

void Core::MainLoop() {
    DirectionalLight::GetInstance();

    int32_t windowWidth = Window::GetInstance()->GetWidth();
    int32_t windowHeight = Window::GetInstance()->GetHeight();

    // TODO: transfer to Rendering class
    GBuffer gBuffer(windowWidth, windowHeight);
    SSAO ssao(windowWidth, windowHeight);
    BlurPass blurPass(windowWidth, windowHeight);
    PostProcess postProcessingFrameBuffer(windowWidth, windowHeight);

    Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize,
                                                                [&postProcessingFrameBuffer, &gBuffer, &ssao, &blurPass](
                                                                        const Event& event) {
        auto& windowResizeEvent = (WindowResizeEvent&) event;
        RenderingAPI::GetInstance()->SetViewport(0, 0,
                                                 windowResizeEvent.GetWidth(),
                                                 windowResizeEvent.GetHeight());

        int32_t windowWidth = windowResizeEvent.GetWidth();
        int32_t windowHeight = windowResizeEvent.GetHeight();

        gBuffer.Resize(windowWidth, windowHeight);
        ssao.Resize(windowWidth, windowHeight);
        blurPass.Resize(windowWidth, windowHeight);
        postProcessingFrameBuffer.Resize(windowWidth, windowHeight);

        Camera::GetInstance()->SetResolution( {windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight()});
        Renderer2D::GetInstance()->SetProjection(windowWidth, windowHeight);
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

        Physics::TickFixedTimeSteps();

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
        Renderer::GetInstance()->LateDraw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        postProcessingFrameBuffer.Draw();
        postProcessingFrameBuffer.CopyDepthBuffer(0);

        Gizmos::DrawGizmos();
        Renderer2D::GetInstance()->Draw();

#ifdef DEBUG
        RenderImGUI();
#endif

        Window::GetInstance()->SwapBuffers();
        Window::GetInstance()->PollEvents();

        Time::CapFPS();
    }
}

#ifdef DEBUG
void Core::RenderImGUI() const {
    ImGui::Begin("FPS");
    ImGui::Text("Framerate: %.3f (%.1f FPS)", Time::GetTrueDeltaSeconds(), 1 / Time::GetTrueDeltaSeconds());
    ImGui::Text("Time: %.3f", Time::GetSeconds());
    ImGui::End();

    ImGui::Begin("Input");
    float forward = 0.f;
    float right = 0.f;

    forward += Input::GetActionStrength("forward_one");
    forward -= Input::GetActionStrength("backward_one");

    right += Input::GetActionStrength("right_one");
    right -= Input::GetActionStrength("left_one");

    ImGui::Text("forward: %f", forward);
    ImGui::Text("right: %f", right);

    ImGui::End();

    ImGui::Begin("Testing");
    ImGui::Separator();

    ImGui::Separator();
    ImGui::Text("Camera");
    glm::vec3 position = Camera::GetInstance()->GetPosition();
    ImGui::DragFloat3("Camera Position", (float*) &position);
    Camera::GetInstance()->SetPosition(position);

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
