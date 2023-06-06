
#include "Rendering/RenderingAPI.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "Macros.h"

#include "Rendering/Assets/ModelAsset.h"

namespace mlg {
    RenderingAPI* RenderingAPI::instance;

    void RenderingAPI::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Rendering Api");

        instance = new RenderingAPI();

        bool gladLoaderStatus = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        MLG_ASSERT_MSG(gladLoaderStatus, "Failed to initialize GLAD");

        std::cout << "\nOpenGL Info:\n"
        << "\tVendor: " << glGetString(GL_VENDOR) << "\n"
        << "\tRenderer: " << glad_glGetString(GL_RENDERER) << "\n"
        << "\tVersion: " << glad_glGetString(GL_VERSION) << "\n\n";


#ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(RenderingAPI::OpenGlMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_STENCIL_TEST);

        instance->screenSpaceQuad.Initialize();
    }

    void RenderingAPI::OpenGlMessageCallback(unsigned int source, unsigned int type, unsigned int id,
                                             unsigned int severity, int length, const char* message,
                                             const void* userParam) {
        switch (severity) {
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
            default:
                SPDLOG_ERROR("OpenGL: {}", message);
                return;
        }
    }

    void RenderingAPI::Stop() {
        SPDLOG_INFO("Stopping Rendering Api");

        delete instance;
        instance = nullptr;
    }

    void RenderingAPI::SetViewport(int32_t x, int32_t y, int32_t width, int32_t height) {
        glViewport(x, y, width, height);
    }

    void RenderingAPI::SetClearColor(const glm::vec4& color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderingAPI::Clear() {
        glUseProgram(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    RenderingAPI* RenderingAPI::GetInstance() {
        return instance;
    }

    void RenderingAPI::DrawScreenSpaceQuad() {
        screenSpaceQuad.Draw();
    }

    void RenderingAPI::SetDefaultFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


}// namespace mlg