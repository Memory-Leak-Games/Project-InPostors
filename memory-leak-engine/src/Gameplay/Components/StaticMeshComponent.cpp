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
            : SceneComponent(owner, name), model(model), material(material), wasDirty(true) {
        GetTransform().onTransformationChange.append([this]() {
            this->wasDirty = true;
        });
    }

    void StaticMeshComponent::Start() {
        thisAsRenderable = std::dynamic_pointer_cast<Renderable>(
                ComponentManager::GetByRawPointer(this).lock());

        Renderer::GetInstance()->AddRenderable(thisAsRenderable);
    }

    void StaticMeshComponent::Draw(struct Renderer* renderer) {
        ZoneScopedN("Draw StaticMesh");
        Camera* camera = renderer->GetCurrentCamera();

        if (camera->GetWasViewDirty() || camera->GetWasProjectionDirty() || wasDirty)
        {
            ZoneScopedN("Calculate Matrices");
            worldMatrix = GetTransform().GetWorldMatrix();
            modelToView = CommonUniformBuffer::GetUniforms().view * worldMatrix;
            modelToScreen = CommonUniformBuffer::GetUniforms().projection * modelToView;

            modelToViewNormals = glm::mat3(glm::transpose(glm::inverse(modelToView)));
        }

        {
            ZoneScopedN("Send Matrices");
            material->Activate();
            //        material->GetShaderProgram()->SetMat4F("world", worldMatrix);
            material->GetShaderProgram()->SetMat4F("modelToView", modelToView);
            material->GetShaderProgram()->SetMat4F("modelToScreen", modelToScreen);
            material->GetShaderProgram()->SetMat3F("modelToViewNormals", modelToViewNormals);
        }

        {
            ZoneScopedN("Draw Model");
            renderer->DrawModel(model.get());
        }

        wasDirty = false;
    }

    void StaticMeshComponent::DrawShadowMap(struct Renderer* renderer, struct ShaderProgram* shaderProgram) {
        glm::mat4 worldMatrix = GetTransform().GetWorldMatrix();
        glm::mat4 modelToLight = DirectionalLight::GetInstance()->GetSun().lightSpaceMatrix * worldMatrix;

        shaderProgram->SetMat4F("modelToLight", modelToLight);
        renderer->DrawModel(model.get());
    }

    void StaticMeshComponent::Stop() {
        if (thisAsRenderable.expired())
            return;

        Renderer::GetInstance()->RemoveRenderable(thisAsRenderable);
    }

    StaticMeshComponent::~StaticMeshComponent() {}


} // mlg