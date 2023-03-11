#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>

#include "Mesh/ModelRenderer.h"
#include "Nodes/Node.h"


namespace mlg {
    class CoreEngine {
    private:
        GLFWwindow* window;

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

        static CoreEngine* GetInstance();
        static void Stop();

        void PrepareScene();

        int32_t MainLoop();

        GLFWwindow* GetWindow() const;

        Node* GetSceneRoot();

        ModelRenderer* GetRenderer();

        friend class CameraNode;

    private:

        int32_t Init();
        int32_t InitializeWindow(const std::string& WindowName);
        void InitializeImGui(const char* glslVersion);

        void CheckGLErrors();

        static void GLFWErrorCallback(int error, const char* description);
    };
}// namespace mlg
