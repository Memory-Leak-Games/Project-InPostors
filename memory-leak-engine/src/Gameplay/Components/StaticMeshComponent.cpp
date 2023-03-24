#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/ComponentManager.h"

#include "Rendering/Renderer.h"

namespace mlg {
    StaticMeshComponent::StaticMeshComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                                             const std::shared_ptr<Model>& model) : SceneComponent(owner, name),
                                                                                    model(model) {
    }

    void StaticMeshComponent::Start() {
        std::weak_ptr<Renderable> thisAsRenderable = std::dynamic_pointer_cast<Renderable>(
                ComponentManager::GetByRawPointer(this).lock());

        Renderer::GetInstance()->AddRenderable(thisAsRenderable);
    }

    void StaticMeshComponent::Draw(struct Renderer* renderer) {
        model->GetShader()->Activate();
        model->GetShader()->SetMat4F("World", GetTransform().GetWorldMatrix());
        model->Draw();
    }

} // mlg