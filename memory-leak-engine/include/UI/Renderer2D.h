#pragma once

namespace mlg {
    class Renderer2D {
        static Renderer2D* instance;

        std::vector<std::weak_ptr<class Renderable2D>> renderables;

    public:
        static void Initialize();
        static void Stop();

        static Renderer2D* GetInstance();

        void Draw();

        void AddRenderable(std::weak_ptr<Renderable2D> renderable);
        void RemoveRenderable(std::weak_ptr<Renderable2D> renderable);
    };
}
