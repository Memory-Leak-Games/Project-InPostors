#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>

#include "Nodes/Node.h"
#include "RenderingLayer/ModelRenderer.h"
#include "Window.h"


namespace mlg {
    class CoreEngine {
    private:
        Window* mainWindow;

        class CameraNode* currentCamera;

        std::shared_ptr<class Lights> sceneLight;
        Node sceneRoot;
        ModelRenderer renderer;

        static CoreEngine* instance;

        explicit CoreEngine();
        ~CoreEngine(){};

    public:
        CoreEngine(CoreEngine const &) = delete;
        void operator=(const CoreEngine &) = delete;

        static int32_t Initialize(Window* mainWindow);
        static CoreEngine* GetInstance();
        static void Stop();

        int32_t MainLoop();

        Node* GetSceneRoot();

        ModelRenderer* GetRenderer();

        friend class CameraNode;

    private:
        int32_t Init();
        void InitializeImGui(const char* glslVersion);

        void CheckGLErrors();

        static void GLFWErrorCallback(int error, const char* description);
    };
}// namespace mlg
