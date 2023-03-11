#pragma once

#include <memory>

#include <GLFW/glfw3.h>
#include <cstdint>

#include "Mesh/ModelRenderer.h"
#include "Nodes/Node.h"
#include "glm/gtc/constants.hpp"
#include "glm/vec3.hpp"

namespace mlg {
    class CoreEngine {
    private:
        GLFWwindow* window;

        class CameraNode* currentCamera;

        std::shared_ptr<class Lights> sceneLight;
        Node sceneRoot;
        ModelRenderer renderer;

    public:
        explicit CoreEngine();

        virtual ~CoreEngine();

        int32_t Init();

        void PrepareScene();

        int32_t MainLoop();

        GLFWwindow* GetWindow() const;

        friend class CameraNode;

    private:
        void Stop();

        static void GLFWErrorCallback(int error, const char* description);

        int32_t InitializeWindow(const std::string& WindowName);

        void InitializeImGui(const char* glslVersion);

        void CheckGLErrors();
    };
}// namespace mlg
