//
// Created by LiptonLeon on 02.04.2023.
//

#include "UI/Renderer2D.h"
#include "UI/Renderable2D.h"
#include "spdlog/spdlog.h"

namespace mlg {

    Renderer2D* Renderer2D::instance;

    void Renderer2D::Initialize() {
        if (instance != nullptr)
            return;

        instance = new Renderer2D();

        SPDLOG_INFO("Initializing Renderer2D");
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
        SPDLOG_INFO(renderables.size());
        for (auto& renderable : renderables) {
            if (renderable.expired())
                continue;
            renderable.lock()->Draw();
        }
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

}
