#include "Core/Window.h"

#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <utility>

#include "GLFW/glfw3.h"
#include "Macros.h"

#ifdef DEBUG
#include "backends/imgui_impl_glfw.h"
#endif

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

#include "magic_enum.hpp"
#include "stb_image.h"

using namespace mlg;

#ifdef DEBUG
void Window::ImGuiInit() {
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
}
#endif

Window* Window::instance;

void Window::WindowErrorCallback(int error, const char* description) {
    SPDLOG_ERROR("GLFW: {}: {}", error, description);
}

void Window::Initialize(std::string title) {
    SPDLOG_INFO("Initializing GLFW");
    glfwSetErrorCallback(Window::WindowErrorCallback);

    bool glfwInitResult = glfwInit();

    MLG_ASSERT_MSG(glfwInitResult, "Failed to initialize GLFW");

    instance = new Window();
    instance->LoadWindowSettings();
    instance->windowSettings.title = std::move(title);

    instance->CreateWindow();
}

void mlg::Window::Update() {
    // Switch between fullscreen and windowed mode
    static bool wasPressedLastFrame = false;
    bool isPressed = glfwGetKey(glfwWindow, GLFW_KEY_F11);

    if (isPressed && !wasPressedLastFrame) {
        if (windowSettings.type == WindowType::Fullscreen)
            SetWindowType(WindowType::Windowed);
        else
            SetWindowType(WindowType::Fullscreen);
    }

    wasPressedLastFrame = isPressed;
}

void mlg::Window::SetIcon(const std::string& path) {
    GLFWimage image;
    image.pixels = stbi_load(
            path.c_str(), &image.width, &image.height,
            0, 4);

    glfwSetWindowIcon(glfwWindow, 1, &image);
    stbi_image_free(image.pixels);
}

void mlg::Window::SetTitle(const std::string& title) {
    glfwSetWindowTitle(glfwWindow, title.c_str());
}

void mlg::Window::CreateWindow() {
    windowData.title = windowSettings.title;
    windowData.width = windowSettings.width;
    windowData.height = windowSettings.height;
    windowData.aspectRatio = (float) windowData.width / (float) windowData.height;

    windowData.vSync = false;

    SPDLOG_INFO("Creating GLFW Window: {} {}x{}",
                instance->windowSettings.title,
                instance->windowSettings.width,
                instance->windowSettings.height);

    instance->SetupWindow();
    instance->SetVerticalSync(instance->windowSettings.vSync);
    instance->SetGamepadMappings();

    isWindowVisible = true;
}

void mlg::Window::SetWindowType(WindowType type) {
    if (windowSettings.type == type)
        return;

    windowSettings.type = type;

    if (!isWindowVisible)
        return;

    int32_t windowWidth = windowSettings.width;
    int32_t windowHeight = windowSettings.height;

    if (type == WindowType::Fullscreen) {
        glm::ivec2 monitorResolution = GetMonitorResolution();
        windowWidth = monitorResolution.x;
        windowHeight = monitorResolution.y;

        SPDLOG_DEBUG("Switching to fullscreen mode: {}x{}",
                     windowWidth, windowHeight);
    }

    glm::ivec2 windowPosition = GetMonitorCenter() -
                                glm::ivec2(windowWidth, windowHeight) / 2;

    glfwSetWindowMonitor(
            glfwWindow, GetMonitor(),
            windowPosition.x, windowPosition.y,
            windowWidth, windowHeight,
            GLFW_DONT_CARE);

    bool showDecorations = windowSettings.type == WindowType::Windowed;
    glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, showDecorations);
}

void mlg::Window::SetResolution(glm::ivec2 resolution) {
    if (windowSettings.width == resolution.x &&
        windowSettings.height == resolution.y)
        return;

    windowSettings.width = resolution.x;
    windowSettings.height = resolution.y;

    if (!isWindowVisible)
        return;

    glfwSetWindowSize(glfwWindow, resolution.x, resolution.y);
    WindowResizeEvent event(resolution.x, resolution.y);
    windowData.eventDispatcher.dispatch(event.GetEventType(), event);
}

int32_t Window::SetupWindow() {
    SetWindowContext();

    GLFWmonitor* monitor = GetMonitor();

    glfwWindow = glfwCreateWindow(
            windowData.width, windowData.height,
            windowData.title.c_str(), monitor, nullptr);

    MLG_ASSERT_MSG(glfwWindow != nullptr, "Failed to crate window");

    SetWindowSettings();

    glfwMakeContextCurrent(glfwWindow);
    glfwSetWindowUserPointer(glfwWindow, (void*) &windowData);
    SetupCallbacks();

    glfwSwapInterval(false);

    return 0;
}

void mlg::Window::LoadWindowSettings() {
    std::string path;

    if (std::filesystem::exists(VIDEO_SETTINGS_PATH))
        path = VIDEO_SETTINGS_PATH;
    else
        path = DEFAULT_VIDEO_SETTINGS_PATH;

    std::ifstream videoSettingsFile(path);
    nlohmann::json settingsJson = nlohmann::json::parse(videoSettingsFile);

    std::string windowType = settingsJson["WindowType"];
    windowSettings.type = magic_enum::enum_cast<WindowType>(windowType).value();

    windowSettings.width = settingsJson["ResolutionWidth"];
    windowSettings.width = std::max(windowSettings.width, 640);

    windowSettings.height = settingsJson["ResolutionHeight"];
    windowSettings.height = std::max(windowSettings.height, 480);

    windowSettings.lockSize = settingsJson["LockSize"];
    windowSettings.vSync = settingsJson["VSync"];
}

glm::ivec2 mlg::Window::GetMonitorResolution() const {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    return {mode->width, mode->height};
}

glm::ivec2 mlg::Window::GetMonitorCenter() const {
    return GetMonitorResolution() / 2;
}

void Window::SetWindowContext() const {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);
}

void Window::SetWindowSettings() const {
    if (windowSettings.type == WindowType::Borderless)
        glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED,
                            GLFW_FALSE);

    if (windowSettings.lockSize)
        glfwSetWindowSizeLimits(
                glfwWindow, windowData.width,
                windowData.height, windowData.width,
                windowData.height);
    else
        glfwSetWindowSizeLimits(
                glfwWindow, 640, 480,
                GLFW_DONT_CARE, GLFW_DONT_CARE);
}

GLFWmonitor* Window::GetMonitor() const {
    if (windowSettings.type == WindowType::Fullscreen)
        return glfwGetPrimaryMonitor();
    else
        return nullptr;
}

void Window::SetupCallbacks() {
    SPDLOG_INFO("Setting window callbacks");

    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int32_t width, int32_t height) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);
        windowData->width = width;
        windowData->height = height;
        windowData->aspectRatio = (float) width / float(height);

        WindowResizeEvent event(width, height);

        windowData->eventDispatcher.dispatch(event.GetEventType(), event);
    });

    glfwSetWindowFocusCallback(glfwWindow, [](GLFWwindow* window, int isWindowFocused) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);
        WindowFocusEvent event(isWindowFocused == GLFW_TRUE);

        windowData->eventDispatcher.dispatch(event.GetEventType(), event);
    });

    glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* window) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);
        WindowCloseEvent event;

        windowData->eventDispatcher.dispatch(event.GetEventType(), event);
    });

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);

        switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event{key, false};
                windowData->eventDispatcher.dispatch(event.GetEventType(), event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event{key};
                windowData->eventDispatcher.dispatch(event.GetEventType(), event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event{key, true};
                windowData->eventDispatcher.dispatch(event.GetEventType(), event);
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);

        switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event{button};
                windowData->eventDispatcher.dispatch(event.GetEventType(), event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event{button};
                windowData->eventDispatcher.dispatch(event.GetEventType(), event);
                break;
            }
        }
    });

    glfwSetScrollCallback(glfwWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);
        MouseScrolledEvent event{(float) xOffset, (float) yOffset};
        windowData->eventDispatcher.dispatch(event.GetEventType(), event);
    });

    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* window, double xPos, double yPos) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);
        MouseMovedEvent event{(float) xPos, (float) yPos};
        windowData->eventDispatcher.dispatch(event.GetEventType(), event);
    });
}

void Window::Stop() {
    SPDLOG_INFO("Destroying GLFW Window: {}", instance->windowData.title);
    glfwDestroyWindow(instance->glfwWindow);
    glfwTerminate();
}

Window* Window::Get() {
    return instance;
}

Window::Window()
    : glfwWindow(nullptr) {}

void Window::SetWindowHint(int hint, int value) {
    glfwWindowHint(hint, value);
}

void Window::SetVerticalSync(bool isEnabled) {
    if (windowData.vSync == isEnabled)
        return;

    glfwSwapInterval(isEnabled);
    windowData.vSync = isEnabled;
}

bool Window::GetVerticalSync() {
    return windowData.vSync;
}

void Window::SwapBuffers() {
    glfwSwapBuffers(glfwWindow);
}

void Window::PollEvents() {
    glfwPollEvents();
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(glfwWindow);
}

eventpp::EventDispatcher<EventType, void(const Event&), EventPolicies>* Window::GetEventDispatcher() {
    return &windowData.eventDispatcher;
}

int32_t Window::GetWidth() {
    return windowData.width;
}

int32_t Window::GetHeight() {
    return windowData.height;
}

void* Window::GetNativeWindowHandle() {
    return glfwWindow;
}

float Window::GetAspectRatio() {
    return windowData.aspectRatio;
}

void mlg::Window::SetGamepadMappings() const {
    std::fstream gamepadDb("res/config/gamecontrollerdb.txt");
    std::stringstream gamepadDbStream;
    gamepadDbStream << gamepadDb.rdbuf();

    glfwUpdateGamepadMappings(gamepadDbStream.str().c_str());
}
