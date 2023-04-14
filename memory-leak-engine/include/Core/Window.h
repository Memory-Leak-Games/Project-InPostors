#pragma once

#include "Events/Event.h"
#include "GLFW/glfw3.h"

namespace mlg {

    class Window {
    private:
        static Window* instance;
        GLFWwindow* glfwWindow;

        struct WindowData {
            std::string title;
            int32_t width;
            int32_t height;
            float aspectRatio;
            bool vSync;

            eventpp::EventDispatcher<EventType, void (const Event &), EventPolicies> eventDispatcher;
        };

        WindowData windowData;
    public:
        Window() = delete;

        static void Initialize(std::string title, int32_t width, int32_t height);
        static void Stop();

        static Window* GetInstance();

        virtual void* GetNativeWindowHandle();

        virtual bool ShouldClose();

        virtual void SetWindowHint(int hint, int value);
        virtual void SetVerticalSync(bool isEnabled);
        virtual bool GetVerticalSync();

        virtual int32_t GetWidth();
        virtual int32_t GetHeight();
        virtual float GetAspectRatio();

        virtual eventpp::EventDispatcher<EventType, void (const Event &), EventPolicies>* GetEventDispatcher();

        virtual void SwapBuffers();
        virtual void PollEvents();

#ifdef DEBUG
        virtual void ImGuiInit();
#endif

    private:
        Window(std::string title, int32_t width, int32_t height, float aspectRatio);

        virtual int32_t SetupWindow();
        void SetupCallbacks();

        static void WindowErrorCallback(int error, const char* description);
    };
}
