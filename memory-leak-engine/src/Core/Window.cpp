#include "Core/Window.h"

#include <sstream>
#include <utility>
#include <fstream>

#include "Core/Settings/SettingsManager.h"

#include "GLFW/glfw3.h"
#include "Macros.h"

#ifdef DEBUG
#include "backends/imgui_impl_glfw.h"
#endif


#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include "magic_enum.hpp"
#include "Core/Settings/SettingsManager.h"

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

    auto width = SettingsManager::Get<int32_t>(SettingsType::Video, "ResolutionWidth");
    auto height = SettingsManager::Get<int32_t>(SettingsType::Video, "ResolutionHeight");

    SPDLOG_INFO("Creating GLFW Window: {} {}x{}", title, width, height);

    float aspectRatio = (float) width / (float) height;

    instance = new Window(std::move(title), width, height, aspectRatio);
    instance->SetupWindow();

    instance->SetVerticalSync(SettingsManager::Get<bool>(SettingsType::Video, "VSync"));

    instance->SetGamepadMappings();
}



int32_t Window::SetupWindow() {
    SPDLOG_INFO("Window Setup");
    SetWindowContext();

    SPDLOG_INFO("Creating Window");

    GLFWmonitor* monitor = GetMonitor();

    glfwWindow = glfwCreateWindow(windowData.width, windowData.height, windowData.title.c_str(), monitor, nullptr);

    MLG_ASSERT_MSG(glfwWindow != nullptr, "Failed to crate window");

    SetWindowSettings();

    glfwMakeContextCurrent(glfwWindow);
    glfwSetWindowUserPointer(glfwWindow, (void*) &windowData);
    SetupCallbacks();

    glfwSwapInterval(false);

    return 0;
}

void Window::SetWindowContext() const {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);
}

void Window::SetWindowSettings() const {
    auto windowType = SettingsManager::Get<std::string>(SettingsType::Video, "WindowType");
    if (windowType == "borderless")
        glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_FALSE);

    if (SettingsManager::Get<bool>(SettingsType::Video, "LockSize"))
        glfwSetWindowSizeLimits(glfwWindow, windowData.width, windowData.height, windowData.width, windowData.height);
    else
        glfwSetWindowSizeLimits(glfwWindow, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

GLFWmonitor *Window::GetMonitor() {
    auto windowType = SettingsManager::Get<std::string>(SettingsType::Video, "WindowType");
    if (windowType == "fullscreen")
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
        windowData->aspectRatio = (float) width / float (height);

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

Window::Window(std::string title, int32_t width, int32_t height, float aspectRatio)
    : glfwWindow(nullptr) {
    windowData.title = std::move(title);
    windowData.width = width;
    windowData.height = height;
    windowData.aspectRatio = aspectRatio;

    windowData.vSync = false;
}

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
