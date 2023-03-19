#include "Core/Core.h"

#ifdef DEBUG

#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#endif

#include "GameplayLayer/Nodes/ModelNode.h"
#include "LowLevelRenderer/Camera.h"
#include "LowLevelRenderer/Gizmos/Gizmo.h"
#include "LowLevelRenderer/Lights.h"
#include "LowLevelRenderer/Model.h"
#include "Macros.h"
#include "include/Core/MouseHandler.h"

#include "Core/Time.h"
#include "Core/Window.h"
#include "Core/HID/Input.h"

#include "GameplayLayer/Nodes/FreeCameraNode.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

using namespace mlg;

Core* Core::instance;

int32_t Core::Init() {
    // TODO: Move to renderer Layer
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        SPDLOG_ERROR("Failed to initialize GLAD!");
        return 1;
    }
    SPDLOG_DEBUG("Successfully initialized OpenGL loader!");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

#ifdef DEBUG
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

#ifdef DEBUG
    Window::GetInstance()->ImGuiInit();
#endif

    ImGui_ImplOpenGL3_Init("#version 460");

    // Setup style
    ImGui::StyleColorsDark();
#endif

    Gizmo::Initialize();

    return 0;
}

int32_t Core::MainLoop() {

#ifdef DEBUG
    CheckGLErrors();
#endif


    // TODO: Remove this
    sceneLight = std::make_shared<Lights>();
    float mouseX, mouseY;

    Key::KeyCode lastKey;

    Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::MouseMoved,
                                                                [&mouseX, &mouseY](const Event& event) {
                                                                    MouseMovedEvent mouseMovedEvent = (const MouseMovedEvent&) event;
                                                                    mouseX = mouseMovedEvent.GetX();
                                                                    mouseY = mouseMovedEvent.GetY();
                                                                });

    Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::KeyPressed, [&lastKey](const Event& event) {
        KeyPressedEvent keyPressedEvent = (const KeyPressedEvent&) event;
        lastKey = keyPressedEvent.GetKeyCode();
    });

    auto begin = std::chrono::high_resolution_clock::now();
    while (!Window::GetInstance()->ShouldClose()) {
        Time::UpdateStartFrameTime();

        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef DEBUG
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif

        Input::Update();

        glViewport(0, 0, Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight());

        sceneRoot.GetLocalTransform()->SetRotation(glm::quat({0, Time::GetSeconds(), 0}));

        sceneRoot.Update(Time::GetSeconds(), Time::GetDeltaSeconds());
        sceneRoot.CalculateWorldTransform();
        sceneRoot.Draw();

        renderer.Draw(this);
        sceneLight->DrawGizmos();

#ifdef DEBUG

        ImGui::Begin("FPS");
        ImGui::Text("Framerate: %.3f (%.1f FPS)", Time::GetTrueDeltaSeconds(), 1 / Time::GetTrueDeltaSeconds());
        ImGui::Text("Time: %.3f", Time::GetSeconds());

        ImGui::Separator();

        for (const std::string& action : {"test_button", "test_axis"})
        {
            float testFloat = Input::GetActionStrength(action);
            bool isTestPressed = Input::IsActionPressed(action);
            bool isTestJustPressed = Input::IsActionPressed(action);
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

    return 0;
}

Core::Core() : sceneRoot() {
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

void Core::CheckGLErrors() {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
        SPDLOG_ERROR("OpenGL error: {}", error);
}

Node* Core::GetSceneRoot() {
    return &sceneRoot;
}

ModelRenderer* Core::GetRenderer() {
    return &renderer;
}

int32_t Core::Initialize() {

    int32_t returnCode = 0;
    if (Core::instance == nullptr) {
        Core::instance = new Core();

        returnCode = Core::instance->Init();
    }

    return returnCode;
}

Core* Core::GetInstance() {
    return Core::instance;
}
