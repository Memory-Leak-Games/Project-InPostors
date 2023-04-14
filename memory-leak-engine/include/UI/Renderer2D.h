#pragma once

namespace mlg {
    class Renderer2D {
    private:
        static Renderer2D* instance;

        std::unique_ptr<class FrameBuffer> frameBuffer;

        std::vector<std::weak_ptr<class Renderable2D>> renderables;

        glm::mat4 projection;

        Renderer2D();
    public:
        static void Initialize();
        static void Stop();

        static void OnWindowResize(const class Event& event);

        static Renderer2D* GetInstance();

        void Draw();

        void AddRenderable(std::weak_ptr<Renderable2D> renderable);
        void RemoveRenderable(std::weak_ptr<Renderable2D> renderable);

        glm::mat4 GetProjection() const;

    private:
        void SetProjection(int32_t windowWidth, int32_t windowHeight);
    };
}
