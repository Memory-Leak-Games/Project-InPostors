#include "Core/Window.h"

#include <utility>

#include "Macros.h"

#ifdef DEBUG
#include "imgui_impl/imgui_impl_glfw.h"
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

void Window::Initialize(std::string title, int32_t width, int32_t height) {
    SPDLOG_INFO("Initializing GLFW");
    glfwSetErrorCallback(Window::WindowErrorCallback);

    MLG_ASSERT_MSG(glfwInit(), "Failed to initialize GLFW");

    SPDLOG_INFO("Creating GLFW Window: {} {}x{}", title, width, height);
    instance = new Window(std::move(title), width, height);
    instance->SetupWindow();

    instance->SetVerticalSync(SettingsManager::Get<bool>(SettingsType::Video, "VSync"));
}

int32_t Window::SetupWindow() {
    SPDLOG_INFO("Window Setup");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);

    SPDLOG_INFO("Creating Window");
    glfwWindow = glfwCreateWindow(windowData.width, windowData.height, windowData.title.c_str(), 0, nullptr);
    MLG_ASSERT_MSG(glfwWindow != nullptr, "Failed to crate window");

    glfwMakeContextCurrent(glfwWindow);
    glfwSetWindowUserPointer(glfwWindow, (void*) &windowData);
    SetupCallbacks();

    // TODO: Move this to HID classes
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSwapInterval(false);

    return 0;
}

void Window::SetupCallbacks() {
    SPDLOG_INFO("Setting window callbacks");

    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int32_t width, int32_t height) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);
        windowData->width = width;
        windowData->height = height;

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

Window* Window::GetInstance() {
    return instance;
}

Window::Window(std::string title, int32_t width, int32_t height)
    : glfwWindow(nullptr) {
    windowData.title = std::move(title);
    windowData.width = width;
    windowData.height = height;

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
