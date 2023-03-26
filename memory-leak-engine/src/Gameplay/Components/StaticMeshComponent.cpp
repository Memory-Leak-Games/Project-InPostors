#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/ComponentManager.h"

#include "Rendering/RenderingAPI.h"
#include "Rendering/Renderer.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"

#include "Macros.h"

namespace mlg {
    StaticMeshComponent::StaticMeshComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                                             const std::shared_ptr<ModelAsset>& model,
                                             const std::shared_ptr<MaterialAsset>& material)
            : SceneComponent(owner, name), model(model), material(material) {
    }

    void StaticMeshComponent::Start() {
        thisAsRenderable = std::dynamic_pointer_cast<Renderable>(
                ComponentManager::GetByRawPointer(this).lock());

        Renderer::GetInstance()->AddRenderable(thisAsRenderable);
    }

    void StaticMeshComponent::Draw(struct Renderer* renderer) {
        material->Activate();
        material->GetShaderProgram()->SetMat4F("world", GetTransform().GetWorldMatrix());
        RenderingAPI::GetInstance()->DrawModel(model.get());
    }

    void StaticMeshComponent::Stop() {
        if (thisAsRenderable.expired())
            return;

        Renderer::GetInstance()->RemoveRenderable(thisAsRenderable);
    }

    StaticMeshComponent::~StaticMeshComponent() {}

} // mlg