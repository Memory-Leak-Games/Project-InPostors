#include "include/Rendering/Renderer.h"

#include "Macros.h"

namespace mlg {
    Renderer* Renderer::instance;

    void Renderer::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Renderer");
    }

    void Renderer::Stop() {
        delete instance;
        instance = nullptr;
    }

    void Renderer::Draw(Renderable* renderable) {
        renderable->Draw(instance);
    }

    void Renderer::LateDraw(Renderable* renderable) {
        renderable->LateDraw(instance);
    }
} // mlg