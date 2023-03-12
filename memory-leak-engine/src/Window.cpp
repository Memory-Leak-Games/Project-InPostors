#include "Window.h"

#include <utility>

#include "LoggingMacros.h"

// TODO: ImGUI
#include <imgui_impl/imgui_impl_glfw.h>

// TODO: Remove this
#include "MouseHandler.h"

using namespace mlg;

void Window::WindowErrorCallback(int error, const char* description) {
    SPDLOG_ERROR("GLFW: {}: {}", error, description);
}

Window* Window::CreateWindow(std::string title, Resolution resolution) {
    auto* result = new Window(std::move(title), resolution);
    result->SetupWindow();
    return result;
}

int32_t Window::SetupWindow() {
    glfwSetErrorCallback(Window::WindowErrorCallback);

    if (!glfwInit()) {
        SPDLOG_ERROR("Window: Failsed to initialize GLFW");
        return 1;
    }

    // TODO: Change this
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    glfwWindow = glfwCreateWindow(resolution.width, resolution.height, title.c_str(), nullptr, nullptr);

    if (glfwWindow == nullptr) {
        SPDLOG_ERROR("Window: Failed to create window");
        return 1;
    }

    glfwMakeContextCurrent(glfwWindow);

    // TODO: Change this:
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(glfwWindow, MouseHandler::MouseCallback);

    return 0;
}

void Window::DestroyWindow() {
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

Window::Window(std::string title, Resolution resolution)
    : title(std::move(title)), resolution(resolution), vSync(false), glfwWindow(nullptr) {
}

void Window::SetWindowHint(int hint, int value) {
    glfwWindowHint(hint, value);
}

void Window::SetVerticalSync(bool isEnabled) {
    glfwSwapInterval(isEnabled);
    vSync = isEnabled;
}

bool Window::GetVerticalSync() {
    return vSync;
}

void Window::SwapBuffers() {
    glfwSwapBuffers(glfwWindow);
}

void Window::PollEvents() {
    glfwPollEvents();
}

Resolution Window::GetResolution() {
    glfwGetFramebufferSize(glfwWindow, &resolution.width, &resolution.height);
    return resolution;
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(glfwWindow);
}
void Window::ImGuiInit() {
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
}
