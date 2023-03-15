#include "include/Core/Window.h"

#include <utility>

#include "Macros.h"

#ifdef DEBUG
#include "imgui_impl/imgui_impl_glfw.h"
#endif

// TODO: Remove this
#include "Events/WindowEvent.h"
#include "include/Core/MouseHandler.h"

using namespace mlg;

void Window::WindowErrorCallback(int error, const char* description) {
    SPDLOG_ERROR("GLFW: {}: {}", error, description);
}

Window* Window::CreateWindow(std::string title, Resolution resolution) {
    SPDLOG_INFO("Initializing GLFW");
    glfwSetErrorCallback(Window::WindowErrorCallback);

    MLG_ASSERT(glfwInit(), "Failed to initialize GLFW");

    SPDLOG_INFO("Creating GLFW Window: {} {}x{}", title, resolution.width, resolution.height);
    auto* result = new Window(std::move(title), resolution);
    result->SetupWindow();

    return result;
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
    glfwSetCursorPosCallback(glfwWindow, MouseHandler::MouseCallback);

    return 0;
}

void Window::SetupCallbacks() {
    SPDLOG_INFO("Setting window callbacks");

    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* glfwWindow, int32_t width, int32_t height) {
        WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(glfwWindow);
        windowData->resolution.width = width;
        windowData->resolution.height = height;

        WindowResizeEvent event(width, height);

        windowData->eventDispatcher.dispatch(event);
    });

    glfwSetWindowFocusCallback(glfwWindow, [](GLFWwindow* glfwWindow, int isWindowFocused){
      WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(glfwWindow);
      WindowFocusEvent event(isWindowFocused == GLFW_TRUE);

      windowData->eventDispatcher.dispatch(event);
    });

    glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* glfwWindow){
      WindowData* windowData = (WindowData*) glfwGetWindowUserPointer(glfwWindow);
      WindowCloseEvent event;

      windowData->eventDispatcher.dispatch(event);
    });
}

void Window::DestroyWindow() {
    SPDLOG_INFO("Destroying GLFW Window: {}", windowData.title);
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
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

#ifdef DEBUG
void Window::ImGuiInit() {
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
}
#endif