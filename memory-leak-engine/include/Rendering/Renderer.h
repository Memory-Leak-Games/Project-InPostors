#pragma once

namespace mlg {

    class Renderer {
    private:
        static Renderer* instance;

        std::vector<std::weak_ptr<class Renderable>> renderables;
        std::vector<std::weak_ptr<class LateRenderable>> lateRenderables;

    public:
        static void Initialize();
        static void Stop();

        static Renderer* GetInstance();

        void Draw(class FrameBuffer* currentFramebuffer);
        void LateDraw();

        void DrawModel(class ModelAsset* model);

        void AddRenderable(const std::weak_ptr<Renderable>& renderable);
        void RemoveRenderable(std::weak_ptr<Renderable> renderable);
    };

} // mlg
