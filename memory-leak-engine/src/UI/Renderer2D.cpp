#include "UI/Renderer2D.h"

#include "Core/Window.h"
#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "UI/Renderable2D.h"
#include "spdlog/spdlog.h"

#include "Rendering/RenderingAPI.h"
#include "Rendering/FrameBuffers/SingleTextureFrameBuffer.h"
#include "Events/WindowEvent.h"

namespace mlg {

    Renderer2D* Renderer2D::instance;

    Renderer2D::Renderer2D() {
        // Setup projection mat manually at game start
        Window* window = Window::GetInstance();
        SetProjection(window->GetWidth(), window->GetHeight());

        auto material = AssetManager::GetAsset<MaterialAsset>("res/materials/simple_fbo_material.json");
        frameBuffer = std::make_unique<SingleTextureFrameBuffer>(window->GetWidth(), window->GetHeight(), material);
    }

    void Renderer2D::Initialize() {
        if (instance != nullptr)
            return;

        instance = new Renderer2D();

        SPDLOG_INFO("Initializing Renderer2D");
        Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize, OnWindowResize);
    }

    void Renderer2D::Stop() {
        SPDLOG_INFO("Stopping Renderer2D");

        delete instance;
        instance = nullptr;
    }

    Renderer2D* Renderer2D::GetInstance() {
        return instance;
    }

    void Renderer2D::Draw() {
        ZoneScopedN("Draw UI");
        TracyGpuZone("Draw UI")

        frameBuffer->Activate();
        frameBuffer->Clear();

        for (auto& renderable : renderables) {
            if (renderable.expired())
                continue;
            renderable.lock()->Draw(this);
        }
        windowSizeDirty = false;

        RenderingAPI::SetDefaultFrameBuffer();

        glEnable(GL_BLEND);
        frameBuffer->Draw();
        glDisable(GL_BLEND);
    }

    void Renderer2D::AddRenderable(std::weak_ptr<Renderable2D> renderable) {
        renderables.push_back(renderable);
    }

    void Renderer2D::RemoveRenderable(std::weak_ptr<Renderable2D> renderable) {
        renderables.erase(std::remove_if(renderables.begin(), renderables.end(),
                                         [&renderable](const std::weak_ptr<Renderable2D>& entry) {
                                             return renderable.lock().get() == entry.lock().get();
                                         }),
                          renderables.end());
    }

    void Renderer2D::SetProjection(int32_t windowWidth, int32_t windowHeight) {
        Renderer2D::windowWidth = windowWidth;
        Renderer2D::windowHeight = windowHeight;
        Renderer2D::uiScale = windowHeight / 720.f;

        aspectRatio = (float) windowWidth / (float) windowHeight;

        projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
    }

    glm::mat4 Renderer2D::GetProjection() const {
        return projection;
    }

    void Renderer2D::OnWindowResize(const Event& event) {
        instance->windowSizeDirty = true;

        auto& windowResizeEvent = (WindowResizeEvent&) event;

        instance->SetProjection(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
        instance->frameBuffer->Resize(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
    }

}
