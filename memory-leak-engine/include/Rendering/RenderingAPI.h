#pragma once

namespace mlg {

    class RenderingAPI {
        static RenderingAPI* instance;

        RenderingAPI() = default;

    public:
        static void Initialize();
        static void Stop();

        void DrawModel(class ModelAsset* model);
        void SetClearColor(const glm::vec4& color);
        void Clear();

        void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);

        static RenderingAPI* GetInstance();
    private:

        static void OpenGlMessageCallback(unsigned source, unsigned type, unsigned id,
                                          unsigned severity, int length, const char* message,
                                          const void* userParam);
    };

}// namespace mlg