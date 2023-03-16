#include "Core/Window.h"

#include <utility>

#include "Macros.h"

#ifdef DEBUG
#include "imgui_impl/imgui_impl_glfw.h"
#endif

// TODO: Remove this
#include "Core/MouseHandler.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include "magic_enum.hpp"

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

void Window::InitWindow(std::string title, Resolution resolution) {
    SPDLOG_INFO("Initializing GLFW");
    glfwSetErrorCallback(Window::WindowErrorCallback);

    MLG_ASSERT(glfwInit(), "Failed to initialize GLFW");

    SPDLOG_INFO("Creating GLFW Window: {} {}x{}", title, resolution.width, resolution.height);
    instance = new Window(std::move(title), resolution);
    instance->SetupWindow();
}

int32_t Window::SetupWindow() {


    // TODO: Change this
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    glfwWindow = glfwCreateWindow(windowData.resolution.width, windowData.resolution.height, windowData.title.c_str(), nullptr, nullptr);
    MLG_ASSERT(glfwWindow != nullptr, "Failed to crate window");

    glfwMakeContextCurrent(glfwWindow);
    glfwSetWindowUserPointer(glfwWindow, (void*) &windowData);
    SetupCallbacks();

    // TODO: Move this to HID classes
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    return 0;
}

void Window::SetupCallbacks() {
    SPDLOG_INFO("Setting window callbacks");

    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int32_t width, int32_t height) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(window);
        windowData->resolution.width = width;
        windowData->resolution.height = height;

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

void Window::DestroyWindow() {
    SPDLOG_INFO("Destroying GLFW Window: {}", instance->windowData.title);
    glfwDestroyWindow(instance->glfwWindow);
    glfwTerminate();
}

Window* Window::GetInstance() {
    return instance;
}

Window::Window(std::string title, Resolution resolution)
    : glfwWindow(nullptr) {
    windowData.title = std::move(title);
    windowData.resolution = resolution;
    windowData.vSync = false;
}

void Window::SetWindowHint(int hint, int value) {
    glfwWindowHint(hint, value);
}

void Window::SetVerticalSync(bool isEnabled) {
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

Window::Resolution Window::GetResolution() {
    return windowData.resolution;
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(glfwWindow);
}


eventpp::EventDispatcher<EventType, void(const Event&), EventPolicies>* Window::GetEventDispatcher() {
    return &windowData.eventDispatcher;
}

