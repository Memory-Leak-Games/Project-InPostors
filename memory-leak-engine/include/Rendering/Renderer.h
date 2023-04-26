#pragma once

namespace mlg {

    class Renderer {
    private:
        static Renderer* instance;

        std::vector<std::weak_ptr<class Renderable>> renderables;
        std::vector<std::weak_ptr<class LateRenderable>> lateRenderables;

        std::unique_ptr<class GBuffer> gBuffer;
        std::unique_ptr<class SSAOFrameBuffer> ssaoFrameBuffer;
        std::unique_ptr<class BlurPass> ssaoBlurPass;
        std::unique_ptr<class PostProcess> postProcess;
        std::unique_ptr<class FrameBuffer> fxaa;

        Renderer() = default;

        class Camera* currentCamera;
    public:
        ~Renderer();

        static void Initialize();
        static void Stop();

        static void OnWindowResize(const class Event& event);

        static Renderer* GetInstance();

        void Draw(class FrameBuffer* currentFramebuffer);
        void LateDraw();

        void DrawFrame();

        void DrawModel(class ModelAsset* model);

        void AddRenderable(const std::weak_ptr<Renderable>& renderable);
        void RemoveRenderable(std::weak_ptr<Renderable> renderable);

        Camera* GetCurrentCamera();
        void SetCurrentCamera(Camera* currentCamera);

    private:
        void SSAOPass();
        void DrawShadowMap();
        void DrawRenderables(FrameBuffer *currentFramebuffer);
        void GeometryPass();
    };

} // mlg
