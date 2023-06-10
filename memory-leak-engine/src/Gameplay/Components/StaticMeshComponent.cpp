#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/ComponentManager.h"

#include "Rendering/Renderer.h"
#include "Rendering/CommonUniformBuffer.h"
#include "Rendering/DirectionalLight.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"

#include "Gameplay/Components/CameraComponent.h"

#include "Macros.h"

namespace mlg {
    StaticMeshComponent::StaticMeshComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                                             const std::shared_ptr<ModelAsset>& model,
                                             const std::shared_ptr<MaterialAsset>& material)
            : SceneComponent(owner, name), model(model), material(material), wasDirty(true), visible(true) {
    }

    void StaticMeshComponent::Start() {
        thisAsRenderable = std::dynamic_pointer_cast<Renderable>(
                ComponentManager::GetByRawPointer(this).lock());

        Renderer::GetInstance()->AddRenderable(thisAsRenderable);
    }

    void StaticMeshComponent::Draw(struct Renderer* renderer) {
        if (!visible)
            return;


        ZoneScopedN("Draw StaticMesh");
        {
            ZoneScopedN("Send Matrices");
            material->Activate();
            material->GetShaderProgram()->SetMat4F("world", GetTransform().GetWorldMatrix());
        }

        {
            ZoneScopedN("Draw Model");
            renderer->DrawModel(model.get());
        }
    }

    void StaticMeshComponent::DrawShadowMap(struct Renderer* renderer, struct ShaderProgram* shaderProgram) {
        if (!visible)
            return;

        const glm::mat4 modelToLight = DirectionalLight::GetInstance()->GetSun().lightSpaceMatrix * GetTransform().GetWorldMatrix();
        shaderProgram->SetMat4F("modelToLight", modelToLight);
        renderer->DrawModel(model.get());
    }

    void StaticMeshComponent::SetVisible(bool value) {
        visible = value;
    }

    void StaticMeshComponent::Stop() {
        if (thisAsRenderable.expired())
            return;

        Renderer::GetInstance()->RemoveRenderable(thisAsRenderable);
    }

    StaticMeshComponent::~StaticMeshComponent() {}


} // mlg