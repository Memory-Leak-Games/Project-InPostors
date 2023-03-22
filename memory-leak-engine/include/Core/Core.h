#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>

#include "Window.h"

namespace mlg {
    class Core {
    private:
        class CameraNode* currentCamera;

        std::shared_ptr<class Lights> sceneLight;

        static Core* instance;

        explicit Core();

    public:
        static void Initialize();
        static void Stop();

        static Core* GetInstance();

        int32_t MainLoop();
    private:

        void CheckGLErrors();
    };
}// namespace mlg
