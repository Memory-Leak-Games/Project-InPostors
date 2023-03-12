#pragma once

#include <GLFW/glfw3.h>

namespace mlg {
    struct Resolution {
        int32_t width;
        int32_t height;
    };

    class Window {
    private:
        GLFWwindow* glfwWindow;

        std::string title;
        Resolution resolution;

        bool vSync;

    public:
        Window() = delete;

        static Window* CreateWindow(std::string title, Resolution resolution);
        virtual int32_t SetupWindow();
        virtual void DestroyWindow();

        virtual bool ShouldClose();

        virtual void SetWindowHint(int hint, int value);
        virtual void SetVerticalSync(bool isEnabled);
        virtual bool GetVerticalSync();
        virtual Resolution GetResolution();

        virtual void ImGuiInit();

        virtual void SwapBuffers();
        virtual void PollEvents();

    private:
        Window(std::string title, Resolution resolution);

        static void WindowErrorCallback(int error, const char* description);
    };
}
