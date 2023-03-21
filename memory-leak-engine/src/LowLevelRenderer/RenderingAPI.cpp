
#include "LowLevelRenderer/RenderingAPI.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Macros.h"

namespace mlg {
    RenderingAPI* RenderingAPI::instance;

    void RenderingAPI::Initialize() {
        if (instance != nullptr)
            return;

        instance = new RenderingAPI();

        MLG_ASSERT(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "Failed to initialize GLAD");

#ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(RenderingAPI::OpenGlMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_LINE_SMOOTH);
    }

    void RenderingAPI::OpenGlMessageCallback(unsigned int source, unsigned int type, unsigned int id,
                                             unsigned int severity, int length, const char* message,
                                             const void* userParam) {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
                SPDLOG_CRITICAL("OpenGL error: {}", message);
                return;
            case GL_DEBUG_SEVERITY_MEDIUM:
                SPDLOG_ERROR("OpenGL error: {}", message);
                return;
            case GL_DEBUG_SEVERITY_LOW:
                SPDLOG_WARN("OpenGL error: {}", message);
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                SPDLOG_INFO("OpenGL: {}", message);
                return;
        }
    }

    void RenderingAPI::Stop() {
        delete instance;
        instance = nullptr;
    }
}// namespace mlg