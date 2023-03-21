#pragma once

namespace mlg {

    class RenderingAPI {
        static RenderingAPI* instance;

    public:
        static void Initialize();
        static void Stop();

        void SetClearColor(const glm::vec4& color);

    private:
        void SetViewport(uint32_t x, uint32_t y, uint32_t widith, uint32_t height);

        static void OpenGlMessageCallback(unsigned source, unsigned type, unsigned id,
                                   unsigned severity, int length, const char* message,
                                   const void* userParam);
    };

}// namespace mlg
