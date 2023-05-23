#include "UI/UIRenderer.h"

#include "Core/Window.h"
#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "UI/UIRenderable.h"
#include "spdlog/spdlog.h"

#include "Rendering/RenderingAPI.h"
#include "Rendering/FrameBuffers/SingleTextureFrameBuffer.h"
#include "Events/WindowEvent.h"

namespace mlg {

    UIRenderer* UIRenderer::instance;

    UIRenderer::UIRenderer() {
        // Setup projection mat manually at game start
        Window* window = Window::GetInstance();
        SetProjection(window->GetWidth(), window->GetHeight());

        auto material = AssetManager::GetAsset<MaterialAsset>("res/materials/simple_fbo_material.json");
        frameBuffer = std::make_unique<SingleTextureFrameBuffer>(window->GetWidth(), window->GetHeight(), material);

        // Setup quad vao and vbo
        const float quad[] = {
                -0.5f, 0.5f,
                -0.5f, -0.5f,
                0.5f, -0.5f,

                -0.5f, 0.5f,
                0.5f, -0.5f,
                0.5f, 0.5f};

        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);

        glNamedBufferData(vbo, sizeof(float) * 6 * 4, nullptr, GL_STATIC_DRAW);
        glNamedBufferSubData(vbo, 0, sizeof(quad), quad);

        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 2 * sizeof(float));
    }

    void UIRenderer::Initialize() {
        SPDLOG_INFO("Initializing UIRenderer");
        if (instance != nullptr)
            return;

        instance = new UIRenderer();

        Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize, OnWindowResize);
    }

    void UIRenderer::Stop() {
        SPDLOG_INFO("Stopping UIRenderer");

        delete instance;
        instance = nullptr;
    }

    UIRenderer* UIRenderer::GetInstance() {
        return instance;
    }

    void UIRenderer::Draw() {
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

    void UIRenderer::AddRenderable(std::weak_ptr<UIRenderable> renderable) {
        renderables.push_back(renderable);
    }

    void UIRenderer::RemoveRenderable(std::weak_ptr<UIRenderable> renderable) {
        renderables.erase(std::remove_if(renderables.begin(), renderables.end(),
                                         [&renderable](const std::weak_ptr<UIRenderable>& entry) {
                                             return renderable.lock().get() == entry.lock().get();
                                         }),
                          renderables.end());
    }

    void UIRenderer::SetProjection(int32_t windowWidth, int32_t windowHeight) {
        UIRenderer::windowWidth = windowWidth;
        UIRenderer::windowHeight = windowHeight;
        UIRenderer::uiScale = windowHeight / 720.f;

        aspectRatio = (float) windowWidth / (float) windowHeight;

        projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
    }

    glm::mat4 UIRenderer::GetProjection() const {
        return projection;
    }

    void UIRenderer::OnWindowResize(const Event& event) {
        instance->windowSizeDirty = true;

        auto& windowResizeEvent = (WindowResizeEvent&) event;

        instance->SetProjection(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
        instance->frameBuffer->Resize(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
    }

}
