#include "CoreEngine.h"

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl/imgui_impl_glfw.h>
#include <imgui_impl/imgui_impl_opengl3.h>

#include "LoggingMacros.h"
#include "include/Mesh/Model.h"
#include "Camera.h"
#include "Nodes/ModelNode.h"
#include "MouseHandler.h"
#include "Lights.h"
#include "Gizmos/Gizmo.h"

#include "Nodes/FreeCameraNode.h"

using namespace mlg;

int32_t CoreEngine::Init()
{
    glfwSetErrorCallback(CoreEngine::GLFWErrorCallback);
    if (!glfwInit())
        return 1;

    const char* GLSLVersion = "#version 460";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Enable MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);

    if (InitializeWindow("Memory Leak Engine") != 0)
        return 1;

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        SPDLOG_ERROR("Failed to initialize GLAD!");
        return 1;
    }
    SPDLOG_DEBUG("Successfully initialized OpenGL loader!");


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, MouseHandler::MouseCallback);

    InitializeImGui(GLSLVersion);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Gizmo::Initialize();

    return 0;
}

int32_t CoreEngine::InitializeWindow(const std::string& WindowName = "MemoryLeakEngine")
{
    window = glfwCreateWindow(1280, 720, WindowName.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        SPDLOG_ERROR("Failed to create OpenGL Window");
        return 1;
    }

    glfwMakeContextCurrent(window);

    // Enable VSync
    glfwSwapInterval(true);

    return 0;
}

void CoreEngine::GLFWErrorCallback(int error, const char* description)
{
    SPDLOG_ERROR("GLFW error {}: {}", error, description);
}

int32_t CoreEngine::MainLoop()
{
    auto startProgramTimePoint = std::chrono::high_resolution_clock::now();
    float previousFrameSeconds = 0;

#ifdef DEBUG
    CheckGLErrors();
#endif

    while (!glfwWindowShouldClose(window))
    {
        // TimeCalculation
        std::chrono::duration<float> timeFromStart = std::chrono::high_resolution_clock::now() - startProgramTimePoint;
        float seconds = timeFromStart.count();
        float deltaSeconds = seconds - previousFrameSeconds;
        previousFrameSeconds = seconds;

        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int displayX, displayY;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &displayX, &displayY);
        glViewport(0, 0, displayX, displayY);

        sceneRoot.Update(this, seconds, deltaSeconds);
        sceneRoot.CalculateWorldTransform();
        sceneRoot.Draw();

        renderer.Draw(this);
        sceneLight->DrawGizmos();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

CoreEngine::CoreEngine() : sceneRoot()
{
}

void CoreEngine::InitializeImGui(const char* glslVersion)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // Setup style
    ImGui::StyleColorsDark();
}

CoreEngine::~CoreEngine()
{
    Stop();
}

void CoreEngine::Stop()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (!window)
        return;

    glfwDestroyWindow(window);
    glfwTerminate();
}

void CoreEngine::CheckGLErrors()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
        SPDLOG_ERROR("OpenGL error: {}", error);
}

void CoreEngine::PrepareScene()
{
    auto camera = std::make_shared<FreeCameraNode>(this);
    sceneRoot.AddChild(camera);
    camera->GetLocalTransform()->SetPosition({0, 0, -20});
    camera->SetActive();

    auto modelShader = std::make_shared<ShaderWrapper>("res/shaders/instanced.vert", "res/shaders/textured_model.frag");

    auto tardisModel = std::make_shared<Model>("res/models/Tardis/tardis.obj", modelShader);
    auto tardisNode = std::make_shared<ModelNode>(tardisModel, &renderer);
    sceneRoot.AddChild(tardisNode);

    sceneLight = std::make_shared<Lights>();
}

GLFWwindow* CoreEngine::GetWindow() const {
    return window;
}
