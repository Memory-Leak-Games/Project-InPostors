#pragma once

#include "Events/Event.h"
#include "GLFW/glfw3.h"

namespace mlg {

    class Window {
    public:
        struct Resolution {
            int32_t width;
            int32_t height;

            [[nodiscard]] std::string ToString() const {
                std::stringstream ss;
                ss << width << "x" << height;
                return ss.str();
            }
        };

    private:
        GLFWwindow* glfwWindow;

        struct WindowData {
            std::string title;
            Resolution resolution;
            bool vSync;

            eventpp::EventDispatcher<EventType, void (const Event &), EventPolicies> eventDispatcher;
        };

        WindowData windowData;

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

        virtual eventpp::EventDispatcher<EventType, void (const Event &), EventPolicies>* GetEventDispatcher();

        virtual void SwapBuffers();
        virtual void PollEvents();

#ifdef DEBUG
        virtual void ImGuiInit();
#endif

    private:
        Window(std::string title, Resolution resolution);

        void SetupCallbacks();

        static void WindowErrorCallback(int error, const char* description);
    };
}
