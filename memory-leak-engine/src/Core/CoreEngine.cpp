#include "Core/CoreEngine.h"

#include <iostream>

#include "RenderingLayer/Gizmos/Gizmo.h"
#include "Macros.h"
#include "GameplayLayer/Nodes/ModelNode.h"
#include "RenderingLayer/Model.h"
#include "include/Core/MouseHandler.h"
#include "include/RenderingLayer/Camera.h"
#include "include/RenderingLayer/Lights.h"

#include "GameplayLayer/Nodes/FreeCameraNode.h"
#include "Core/Time.h"
#include "Core/Window.h"

#ifdef DEBUG
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#endif

using namespace mlg;

CoreEngine* CoreEngine::instance;

int32_t CoreEngine::Init() {
    // TODO: Move to renderer Layer
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
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
    mainWindow->ImGuiInit();
#endif

    ImGui_ImplOpenGL3_Init("#version 460");

    // Setup style
    ImGui::StyleColorsDark();
#endif

    Gizmo::Initialize();

    return 0;
}

void CoreEngine::GLFWErrorCallback(int error, const char* description) {
    SPDLOG_ERROR("GLFW error {}: {}", error, description);
}

int32_t CoreEngine::MainLoop() {

#ifdef DEBUG
    CheckGLErrors();
#endif

    Time::SetStartTimePoint(std::chrono::high_resolution_clock::now());

    // TODO: Remove this
    sceneLight = std::make_shared<Lights>();

    auto begin = std::chrono::high_resolution_clock::now();
    while (!mainWindow->ShouldClose()) {
        Time::SetFrameStartTimePoint(std::chrono::high_resolution_clock::now());

        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef DEBUG
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif

        Window::Resolution resolution = mainWindow->GetResolution();
        glViewport(0, 0, resolution.width, resolution.height);

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
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        mainWindow->SwapBuffers();
        mainWindow->PollEvents();

    }

    return 0;
}

CoreEngine::CoreEngine() : sceneRoot() {
}

void CoreEngine::Stop() {
    SPDLOG_INFO("Stopping CoreEngine");

#ifdef DEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    delete CoreEngine::instance;
    instance = nullptr;
}

void CoreEngine::CheckGLErrors() {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
        SPDLOG_ERROR("OpenGL error: {}", error);
}

Node* CoreEngine::GetSceneRoot() {
    return &sceneRoot;
}

ModelRenderer* CoreEngine::GetRenderer() {
    return &renderer;
}

int32_t CoreEngine::Initialize(Window* mainWindow) {

    int32_t returnCode = 0;
    if (CoreEngine::instance == nullptr) {
        CoreEngine::instance = new CoreEngine();
        CoreEngine::instance->mainWindow = mainWindow;

        returnCode = CoreEngine::instance->Init();
    }

    return returnCode;
}


CoreEngine* CoreEngine::GetInstance() {
    return CoreEngine::instance;
}
