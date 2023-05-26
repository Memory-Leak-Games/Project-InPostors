#pragma once

#include "Gameplay/SceneComponent.h"
#include "Rendering/Renderable.h"

namespace mlg {

    class StaticMeshComponent : public SceneComponent, public Renderable {
    private:
        std::shared_ptr<class ModelAsset> model;
        std::shared_ptr<class MaterialAsset> material;
        std::weak_ptr<Renderable> thisAsRenderable;
        bool visible;

        bool wasDirty;

        glm::mat4 worldMatrix;
        glm::mat4 modelToView;
        glm::mat4 modelToScreen;

        glm::mat3 modelToViewNormals;
    public:
        StaticMeshComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                            const std::shared_ptr<ModelAsset>& model, const std::shared_ptr<MaterialAsset>& material);

        void Start() override;
        void Stop() override;

        void Draw(struct Renderer* renderer) override;

        void DrawShadowMap(struct Renderer* renderer, struct ShaderProgram* shaderProgram) override;

        void SetVisible(bool value);

        ~StaticMeshComponent() override;
    };

} // mlg
