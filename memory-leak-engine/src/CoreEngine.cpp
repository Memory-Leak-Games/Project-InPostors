#include "CoreEngine.h"

#include <imgui_impl/imgui_impl_glfw.h>
#include <imgui_impl/imgui_impl_opengl3.h>

#include "Camera.h"
#include "Gizmos/Gizmo.h"
#include "Lights.h"
#include "LoggingMacros.h"
#include "MouseHandler.h"
#include "Nodes/ModelNode.h"
#include "RenderingLayer/Model.h"

#include "Nodes/FreeCameraNode.h"
#include "Time.h"
#include "Window.h"

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

    InitializeImGui("#version 460");

    Gizmo::Initialize();

    return 0;
}

void CoreEngine::GLFWErrorCallback(int error, const char* description) {
    SPDLOG_ERROR("GLFW error {}: {}", error, description);
}

int32_t CoreEngine::MainLoop() {
    Time::startTimePoint = std::chrono::high_resolution_clock::now();

#ifdef DEBUG
    CheckGLErrors();
#endif

    // TODO: Remove this
    sceneLight = std::make_shared<Lights>();

    while (!mainWindow->ShouldClose()) {
        Time::frameStartTimePoint = std::chrono::high_resolution_clock::now();

        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Resolution resolution = mainWindow->GetResolution();
        glViewport(0, 0, resolution.width, resolution.height);

        sceneRoot.Update(Time::GetSeconds(), Time::GetDeltaSeconds());
        sceneRoot.CalculateWorldTransform();
        sceneRoot.Draw();

        renderer.Draw(this);
        sceneLight->DrawGizmos();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        mainWindow->SwapBuffers();
        mainWindow->PollEvents();

        Time::lastFrameEndTimePoint = std::chrono::high_resolution_clock::now();
    }

    return 0;
}

CoreEngine::CoreEngine() : sceneRoot() {
}

void CoreEngine::InitializeImGui(const char* glslVersion) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    mainWindow->ImGuiInit();
    ImGui_ImplOpenGL3_Init(glslVersion);

    // Setup style
    ImGui::StyleColorsDark();
}

void CoreEngine::Stop() {
    SPDLOG_INFO("Stopping CoreEngine");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

        LoggingMacros::InitializeSPDLog();
        returnCode = CoreEngine::instance->Init();
    }

    return returnCode;
}


CoreEngine* CoreEngine::GetInstance() {
    return CoreEngine::instance;
}
