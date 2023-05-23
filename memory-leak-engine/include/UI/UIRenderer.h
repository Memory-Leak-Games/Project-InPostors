#pragma once

namespace mlg {
    class UIRenderer {
    private:
        static UIRenderer* instance;

        std::unique_ptr<class FrameBuffer> frameBuffer;
        std::vector<std::weak_ptr<class UIRenderable>> renderables;
        glm::mat4 projection;

        UIRenderer();

    public:
        uint32_t vbo, vao;
        uint32_t windowWidth = 1280;
        uint32_t windowHeight = 720;
        float aspectRatio = (float) windowWidth / (float) windowHeight;
        float uiScale = 1; // Depends on window's height, ui elements should account for that
        bool windowSizeDirty = true;

        static void Initialize();
        static void Stop();

        static void OnWindowResize(const class Event& event);

        static UIRenderer* GetInstance();

        void Draw();

        void AddRenderable(std::weak_ptr<class UIRenderable> renderable);
        void RemoveRenderable(std::weak_ptr<class UIRenderable> renderable);

        glm::mat4 GetProjection() const;

    private:
        void SetProjection(int32_t windowWidth, int32_t windowHeight);
    };
}
