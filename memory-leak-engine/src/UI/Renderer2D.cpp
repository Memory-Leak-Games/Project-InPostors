//
// Created by LiptonLeon on 02.04.2023.
//

#include "UI/Renderer2D.h"

#include "Core/Window.h"
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
        frameBuffer = std::make_unique<SingleTextureFrameBuffer>(window->GetWidth(), window->GetHeight());


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
        frameBuffer->Activate();
        frameBuffer->Clear();

        for (auto& renderable : renderables) {
            if (renderable.expired())
                continue;
            renderable.lock()->Draw(this);
        }

        RenderingAPI::SetDefaultFrameBuffer();
        frameBuffer->Draw();
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
        Renderer2D::uiScale = windowHeight / 1080.0f;

        float aspectRatio = (float) windowWidth / (float) windowHeight;

        projection = glm::ortho(0.0f, 1080 * aspectRatio, 0.0f, 1080.f);
    }

    glm::mat4 Renderer2D::GetProjection() const {
        return projection;
    }

    void Renderer2D::OnWindowResize(const Event& event) {
        auto& windowResizeEvent = (WindowResizeEvent&) event;

        instance->SetProjection(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
        instance->frameBuffer->Resize(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
    }

}
