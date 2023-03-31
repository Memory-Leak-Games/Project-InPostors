#pragma once

namespace mlg {

    class Renderer {
    private:
        static Renderer* instance;

        class FrameBuffer* currentFramebuffer = nullptr;

        std::vector<std::weak_ptr<class Renderable>> renderables;
        std::vector<std::weak_ptr<class LateRenderable>> lateRenderables;
    public:
        static void Initialize();
        static void Stop();

        static Renderer* GetInstance();

        void Draw(FrameBuffer* currentFramebuffer);
        void LateDraw(FrameBuffer* currentFramebuffer);

        void DrawModel(class ModelAsset* model, class MaterialAsset* material);

        void AddRenderable(const std::weak_ptr<Renderable>& renderable);
        void RemoveRenderable(std::weak_ptr<Renderable> renderable);
    };

} // mlg
