#include "Rendering/Renderer.h"

#include "Macros.h"
#include "Core/Window.h"
#include "Rendering/Renderable.h"
#include "Rendering/LateRenderable.h"

#include "Rendering/FrameBuffers/FrameBuffer.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "Rendering/DirectionalLight.h"

#include "Rendering/FrameBuffers/GBuffer.h"
#include "Rendering/FrameBuffers/SSAOFrameBuffer.h"
#include "Rendering/FrameBuffers/BlurPass.h"
#include "Rendering/FrameBuffers/PostProcess.h"
#include "Rendering/RenderingAPI.h"
#include "Events/WindowEvent.h"

namespace mlg {
    Renderer* Renderer::instance;

    Renderer::~Renderer() = default;

    void Renderer::Initialize() {
        if (instance != nullptr)
            return;

        instance = new Renderer();

        SPDLOG_INFO("Initializing Renderer");

        int32_t windowWidth = Window::GetInstance()->GetWidth();
        int32_t windowHeight = Window::GetInstance()->GetHeight();

        instance->gBuffer = std::make_unique<GBuffer>(windowWidth, windowHeight);
        instance->ssaoFrameBuffer = std::make_unique<SSAOFrameBuffer>(windowWidth, windowHeight);
        instance->ssaoBlurPass = std::make_unique<BlurPass>(windowWidth, windowHeight);
        instance->postProcess = std::make_unique<PostProcess>(windowWidth, windowHeight);

        Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize, OnWindowResize);
    }

    void Renderer::OnWindowResize(const Event& event) {
        auto& windowResizeEvent = (WindowResizeEvent&) event;
        int32_t windowWidth = windowResizeEvent.GetWidth();
        int32_t windowHeight = windowResizeEvent.GetHeight();

        instance->gBuffer->Resize(windowWidth, windowHeight);
        instance->ssaoFrameBuffer->Resize(windowWidth, windowHeight);
        instance->ssaoBlurPass->Resize(windowWidth, windowHeight);
        instance->postProcess->Resize(windowWidth, windowHeight);
    }

    void Renderer::Stop() {
        SPDLOG_INFO("Stopping Renderer");

        delete instance;
        instance = nullptr;
    }

    void Renderer::Draw(FrameBuffer* currentFramebuffer) {
        DirectionalLight::GetInstance()->BindShadowMap();
        DirectionalLight::GetInstance()->BindShadowMapShader();

        glCullFace(GL_FRONT);
        for (auto& renderable : renderables) {
            renderable.lock()->DrawShadowMap(this,
                                             DirectionalLight::GetInstance()->GetShadowShaderProgram().lock().get());
        }
        glCullFace(GL_BACK);


        glViewport(0, 0, Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight());
        currentFramebuffer->Activate();
        for (auto& renderable : renderables) {
            renderable.lock()->Draw(this);
        }
    }

    void Renderer::DrawModel(ModelAsset* model) {
        model->Draw();
    }

    void Renderer::LateDraw() {
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

    void Renderer::DrawFrame() {
        gBuffer->Activate();
        gBuffer->Clear();
        Renderer::GetInstance()->Draw(gBuffer.get());

        gBuffer->CopyDepthBuffer(postProcess->GetFbo());

        ssaoFrameBuffer->BindTextureUnits(gBuffer->GetPositionTexture(), gBuffer->GetNormalTexture());
        ssaoFrameBuffer->Draw();
        ssaoBlurPass->BindTextureUnits(ssaoFrameBuffer->GetOutput());
        ssaoBlurPass->BindTextureUnits(ssaoFrameBuffer->GetOutput());
        ssaoBlurPass->Draw();

        postProcess->Activate();

        gBuffer->BindTextures(ssaoBlurPass->GetBlurredTexture());
//        gBuffer.BindTextures(0);
        gBuffer->Draw();
        Renderer::GetInstance()->LateDraw();

        RenderingAPI::SetDefaultFrameBuffer();
        postProcess->Draw();
        postProcess->CopyDepthBuffer(0);
    }

} // mlg