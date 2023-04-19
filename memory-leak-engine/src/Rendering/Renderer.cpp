#include "Rendering/Renderer.h"

#include "Core/Window.h"
#include "Core/Settings/SettingsManager.h"

#include "Events/WindowEvent.h"

#include "Rendering/RenderingAPI.h"

#include "Rendering/Renderable.h"
#include "Rendering/LateRenderable.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "Rendering/DirectionalLight.h"

#include "Rendering/FrameBuffers/FrameBuffer.h"
#include "Rendering/FrameBuffers/GBuffer.h"
#include "Rendering/FrameBuffers/SSAOFrameBuffer.h"
#include "Rendering/FrameBuffers/BlurPass.h"
#include "Rendering/FrameBuffers/PostProcess.h"
#include "Rendering/FrameBuffers/FXAAFrameBuffer.h"

#include "Macros.h"

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
        instance->fxaa = std::make_unique<FXAAFrameBuffer>(windowWidth, windowHeight);

        Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize, OnWindowResize);

        TracyGpuContext;
    }

    void Renderer::OnWindowResize(const Event& event) {
        auto& windowResizeEvent = (WindowResizeEvent&) event;
        int32_t windowWidth = windowResizeEvent.GetWidth();
        int32_t windowHeight = windowResizeEvent.GetHeight();

        instance->gBuffer->Resize(windowWidth, windowHeight);
        instance->ssaoFrameBuffer->Resize(windowWidth, windowHeight);
        instance->ssaoBlurPass->Resize(windowWidth, windowHeight);
        instance->postProcess->Resize(windowWidth, windowHeight);
        instance->fxaa->Resize(windowWidth, windowHeight);
    }

    void Renderer::Stop() {
        SPDLOG_INFO("Stopping Renderer");

        delete instance;
        instance = nullptr;
    }

    void Renderer::Draw(FrameBuffer* currentFramebuffer) {
        DrawShadowMap();
        DrawRenderables(currentFramebuffer);
    }

    void Renderer::DrawRenderables(FrameBuffer *currentFramebuffer) {
        TracyGpuZone("DrawRenderables");

        glViewport(0, 0, Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight());
        currentFramebuffer->Activate();
        for (auto& renderable : renderables) {
            renderable.lock()->Draw(this);
        }
    }

    void Renderer::DrawShadowMap() {
        TracyGpuZone("DrawShadowMap");

        DirectionalLight::GetInstance()->BindShadowMap();
        DirectionalLight::GetInstance()->BindShadowMapShader();

        glCullFace(GL_FRONT);
        for (auto& renderable : renderables) {
            renderable.lock()->DrawShadowMap(this,
                                             DirectionalLight::GetInstance()->GetShadowShaderProgram().lock().get());
        }
        glCullFace(GL_BACK);
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
        ZoneScoped;
        TracyGpuZone("DrawFrame");

        gBuffer->Activate();
        gBuffer->Clear();

        Renderer::GetInstance()->Draw(gBuffer.get());

        gBuffer->CopyDepthBuffer(postProcess->GetFbo());

        SSAOPass();

        bool isFXAAActive = SettingsManager::Get<bool>(SettingsType::Video, "FXAA");

        if (isFXAAActive) {
            fxaa->Activate();
        } else {
            postProcess->Activate();
        }

        gBuffer->Draw();
        Renderer::GetInstance()->LateDraw();

        if (isFXAAActive) {
            TracyGpuZone("FXAA");
            postProcess->Activate();
            fxaa->Draw();
        }

        {
            TracyGpuZone("PostProcess");
            RenderingAPI::SetDefaultFrameBuffer();
            postProcess->Draw();
            postProcess->CopyDepthBuffer(0);
        }
    }

    void Renderer::SSAOPass() {
        if (!SettingsManager::Get<bool>(SettingsType::Video, "SSAO")) {
            gBuffer->BindTextures(0);
            return;
        }
        TracyGpuZone("SSAO");

        ssaoFrameBuffer->BindTextureUnits(gBuffer->GetPositionTexture(), gBuffer->GetNormalTexture());
        ssaoFrameBuffer->Draw();
        ssaoBlurPass->BindTextureUnits(ssaoFrameBuffer->GetOutput());
        ssaoBlurPass->BindTextureUnits(ssaoFrameBuffer->GetOutput());
        ssaoBlurPass->Draw();

        gBuffer->BindTextures(ssaoBlurPass->GetBlurredTexture());
    }

} // mlg