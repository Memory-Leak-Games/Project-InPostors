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
    Renderer *Renderer::instance;

    Renderer::~Renderer() = default;

    void Renderer::Initialize() {
        if (instance != nullptr)
            return;

        instance = new Renderer();

        SPDLOG_INFO("Initializing Renderer");

        int32_t windowWidth = Window::Get()->GetWidth();
        int32_t windowHeight = Window::Get()->GetHeight();

        instance->gBuffer = std::make_unique<GBuffer>(windowWidth, windowHeight);
        instance->ssaoFrameBuffer = std::make_unique<SSAOFrameBuffer>(windowWidth, windowHeight);
        instance->ssaoBlurPass = std::make_unique<BlurPass>(windowWidth, windowHeight);
        instance->postProcess = std::make_unique<PostProcess>(windowWidth, windowHeight);
        instance->fxaa = std::make_unique<FXAAFrameBuffer>(windowWidth, windowHeight);

        Window::Get()->GetEventDispatcher()->appendListener(EventType::WindowResize, OnWindowResize);

        TracyGpuContext;
    }

    void Renderer::OnWindowResize(const Event &event) {
        auto &windowResizeEvent = (WindowResizeEvent &) event;
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

    void Renderer::Draw(FrameBuffer *currentFramebuffer) {
        DrawShadowMap();
        DrawRenderables(currentFramebuffer);
    }

    void Renderer::DrawRenderables(FrameBuffer *currentFramebuffer) {
        ZoneScopedN("Draw Renderables");
        TracyGpuZone("Draw Renderables");

        glViewport(0, 0, Window::Get()->GetWidth(), Window::Get()->GetHeight());
        currentFramebuffer->Activate();
        for (auto &renderable: renderables) {
            renderable.lock()->Draw(this);
        }
    }

    void Renderer::DrawShadowMap() {
        ZoneScopedN("Draw ShadowMap");
        TracyGpuZone("Draw ShadowMap");

        DirectionalLight::GetInstance()->BindShadowMap();
        DirectionalLight::GetInstance()->BindShadowMapShader();

        glCullFace(GL_FRONT);
        for (auto &renderable: renderables) {
            renderable.lock()->DrawShadowMap(this, DirectionalLight::GetInstance()->GetShadowShaderProgram().lock().get());
        }
        glCullFace(GL_BACK);
    }

    void Renderer::DrawModel(ModelAsset *model) {
        model->Draw();
    }

    void Renderer::LateDraw() {
        ZoneScopedN("Late Draw");
        TracyGpuZone("Late Draw");

        for (auto &lateRenderable: lateRenderables) {
            if (lateRenderable.expired())
                continue;

            lateRenderable.lock()->LateDraw(this);
        }
    }

    void Renderer::AddRenderable(const std::weak_ptr<Renderable> &renderable) {
        renderables.push_back(renderable);
    }

    void Renderer::AddLateRenderable(const std::weak_ptr<LateRenderable> &lateRenderable) {
        lateRenderables.push_back(lateRenderable);
    }

    void Renderer::RemoveRenderable(std::weak_ptr<Renderable> renderable) {
        renderables.erase(std::remove_if(renderables.begin(), renderables.end(),
                                         [&renderable](const std::weak_ptr<Renderable> &entry) {
                                             return renderable.lock().get() == entry.lock().get();
                                         }), renderables.end());
    }

    void Renderer::RemoveLateRenderable(std::weak_ptr<LateRenderable> lateRenderable) {
        lateRenderables.erase(std::remove_if(lateRenderables.begin(), lateRenderables.end(),
                                         [&lateRenderable](const std::weak_ptr<LateRenderable> &entry) {
                                             return lateRenderable.lock().get() == entry.lock().get();
                                         }), lateRenderables.end());
    }

    Renderer *Renderer::GetInstance() {
        return instance;
    }

    void Renderer::DrawFrame() {
        ZoneScopedN("Renderer DrawFrame");
        TracyGpuZone("Renderer DrawFrame");

        GeometryPass();

        SSAOPass();

        bool isFXAAActive = SettingsManager::Get<bool>(SettingsType::Video, "FXAA");

        if (isFXAAActive) {
            gBuffer->CopyDepthBuffer(fxaa->GetFbo());
            fxaa->Activate();
        } else {
            gBuffer->CopyDepthBuffer(postProcess->GetFbo());
            postProcess->Activate();
        }

        gBuffer->Draw();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        Renderer::GetInstance()->LateDraw();
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

        if (isFXAAActive) {
            ZoneScopedN("FXAA");
            TracyGpuZone("FXAA");
            postProcess->Activate();
            fxaa->Draw();
        }

        {
            ZoneScopedN("PostProcess");
            TracyGpuZone("PostProcess");
            RenderingAPI::SetDefaultFrameBuffer();
            postProcess->Draw();
//            postProcess->CopyDepthBuffer(0);
        }
    }

    void Renderer::GeometryPass() {
        ZoneScopedN("Geometry Pass");
        TracyGpuZone("Geometry Pass");

        gBuffer->Activate();
        gBuffer->Clear();

        GetInstance()->Draw(gBuffer.get());

    }

    void Renderer::SSAOPass() {
        if (!SettingsManager::Get<bool>(SettingsType::Video, "SSAO")) {
            gBuffer->BindTextures(0);
            return;
        }
        ZoneScopedN("SSAO");
        TracyGpuZone("SSAO");

        ssaoFrameBuffer->BindTextureUnits(gBuffer->GetPositionTexture(), gBuffer->GetNormalTexture());
        ssaoFrameBuffer->Draw();
        ssaoBlurPass->BindTextureUnits(ssaoFrameBuffer->GetOutput());
        ssaoBlurPass->BindTextureUnits(ssaoFrameBuffer->GetOutput());
        ssaoBlurPass->Draw();

        gBuffer->BindTextures(ssaoBlurPass->GetBlurredTexture());
    }

    Camera* Renderer::GetCurrentCamera() {
        return currentCamera;
    }

    void Renderer::SetCurrentCamera(Camera* currentCamera) {
        Renderer::currentCamera = currentCamera;
    }

} // mlg