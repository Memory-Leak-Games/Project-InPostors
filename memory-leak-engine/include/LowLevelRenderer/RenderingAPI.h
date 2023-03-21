#pragma once

namespace mlg {
    class RenderingAPI {
    private:
        static RenderingAPI* instance;

        RenderingAPI() = default;

    public:
        static void Initialize();
        static void Stop();

        void SetClearColor(const glm::vec4& color);
        void Clear();

        static RenderingAPI* GetInstance();
    private:
        static void OpenGlMessageCallback(unsigned int source, unsigned int type, unsigned int id,
                                   unsigned int severity, int length, const char* message,
                                   const void* userParam);
        void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);
    };
}