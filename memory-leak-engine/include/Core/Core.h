#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>

#include "GameplayLayer/Nodes/Node.h"
#include "Window.h"

namespace mlg {
    class Core {
    private:
        class CameraNode* currentCamera;

        std::shared_ptr<class Lights> sceneLight;
        Node sceneRoot;

        static Core* instance;

        explicit Core();

    public:
        Core(Core const&) = delete;
        void operator=(const Core&) = delete;

        static int32_t Initialize();
        static Core* GetInstance();
        static void Stop();

        int32_t MainLoop();

        Node* GetSceneRoot();

        friend class CameraNode;

    private:
        int32_t Init();

        void CheckGLErrors();
    };
}// namespace mlg
