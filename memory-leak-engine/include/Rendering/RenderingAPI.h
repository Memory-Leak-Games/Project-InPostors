#pragma once

#include "Rendering/QuadRenderer.h"

namespace mlg {

    class RenderingAPI {
        static RenderingAPI* instance;
        QuadRenderer screenSpaceQuad;

        RenderingAPI() = default;
    public:
        static void Initialize();
        static void Stop();

        void DrawScreenSpaceQuad();

        void SetClearColor(const glm::vec4& color);
        void Clear();

        void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
        static void SetDefaultFrameBuffer();

        static RenderingAPI* GetInstance();
    private:

        static void OpenGlMessageCallback(unsigned source, unsigned type, unsigned id,
                                          unsigned severity, int length, const char* message,
                                          const void* userParam);
    };

}// namespace mlg