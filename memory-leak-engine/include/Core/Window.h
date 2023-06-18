#pragma once

#include "Events/Event.h"
#include "GLFW/glfw3.h"

#define DEFAULT_VIDEO_SETTINGS_PATH "res/config/settings/video.json"
#define VIDEO_SETTINGS_PATH "res/runtime_files/video.json"

namespace mlg {

    enum class WindowType {
        Windowed = 0,
        Borderless = 1,
        Fullscreen = 2,
    };

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
            bool fullscreen;

            eventpp::EventDispatcher<EventType, void(const Event&), EventPolicies> eventDispatcher;
        };

        WindowData windowData;

        struct WindowSettings {
            std::string title;
            WindowType type;
            int32_t width;
            int32_t height;
            bool lockSize;
            bool vSync;
        } windowSettings;

        bool isWindowVisible = false;

    public:
        Window();

        static void Initialize(std::string title);
        static void Stop();

        static Window* Get();

        virtual void* GetNativeWindowHandle();

        virtual bool ShouldClose();

        virtual void SetWindowHint(int hint, int value);
        virtual void SetVerticalSync(bool isEnabled);
        virtual void SetWindowType(WindowType type);
        virtual void SetResolution(glm::ivec2 resolution);
        virtual bool GetVerticalSync();

        virtual int32_t GetWidth();
        virtual int32_t GetHeight();
        virtual float GetAspectRatio();

        virtual eventpp::EventDispatcher<EventType, void(const Event&), EventPolicies>* GetEventDispatcher();

        virtual void SwapBuffers();
        virtual void PollEvents();
        virtual void Update();

        virtual void SetIcon(const std::string& path);
        virtual void SetTitle(const std::string& title);

#ifdef DEBUG
        virtual void ImGuiInit();
#endif

    private:
        virtual void CreateWindow();
        virtual int32_t SetupWindow();
        void SetupCallbacks();

        static void WindowErrorCallback(int error, const char* description);

        void SetWindowContext() const;
        void SetWindowSettings() const;
        GLFWmonitor* GetMonitor() const;

        void SetGamepadMappings() const;

        void LoadWindowSettings();

        glm::ivec2 GetMonitorResolution() const;
        glm::ivec2 GetMonitorCenter() const;
    };
}// namespace mlg
