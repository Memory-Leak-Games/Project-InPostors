#include "include/Rendering/Renderer.h"

#include "Macros.h"
#include "Rendering/Renderable.h"
#include "Rendering/LateRenderable.h"

#include "Rendering/FrameBuffers/FrameBuffer.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

namespace mlg {
    Renderer* Renderer::instance;

    void Renderer::Initialize() {
        if (instance != nullptr)
            return;

        instance = new Renderer();

        SPDLOG_INFO("Initializing Renderer");
    }

    void Renderer::Stop() {
        SPDLOG_INFO("Stopping Renderer");

        delete instance;
        instance = nullptr;
    }

    void Renderer::Draw(FrameBuffer* currentFramebuffer) {
        this->currentFramebuffer = currentFramebuffer;

        for (auto& renderable : renderables) {
            renderable.lock()->Draw(this);
        }
    }

    void Renderer::DrawModel(ModelAsset* model, MaterialAsset* material) {
        // Render to shadowmap

        currentFramebuffer->Activate();
        material->Activate();
        model->Draw();
    }

    void Renderer::LateDraw(FrameBuffer* currentFramebuffer) {
        this->currentFramebuffer = currentFramebuffer;

        for (auto& lateRenderable : lateRenderables) {
            lateRenderable.lock()->LateDraw(this);
        }
    }

    void Renderer::AddRenderable(const std::weak_ptr<Renderable>& renderable) {
        renderables.push_back(renderable);
    }

    void Renderer::RemoveRenderable(std::weak_ptr<Renderable> renderable) {
        renderables.erase(std::remove_if(renderables.begin(), renderables.end(),
                                         [&renderable](const std::weak_ptr<Renderable>& entry) {
                                             return renderable.lock().get() == entry.lock().get();
                                         }), renderables.end());
    }

    Renderer* Renderer::GetInstance() {
        return instance;
    }

} // mlg