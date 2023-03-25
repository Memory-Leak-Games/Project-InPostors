#pragma once

#include "Gameplay/SceneComponent.h"
#include "Rendering/Renderable.h"

namespace mlg {

    class StaticMeshComponent : public SceneComponent, public Renderable {
    private:
        std::shared_ptr<class ModelAsset> model;
        std::shared_ptr<class MaterialAsset> material;
        std::weak_ptr<Renderable> thisAsRenderable;
    public:
        StaticMeshComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                            const std::shared_ptr<ModelAsset>& model, const std::shared_ptr<MaterialAsset>& material);

        void Start() override;
        void Stop() override;

        void Draw(struct Renderer* renderer) override;

        ~StaticMeshComponent() override;
    };

} // mlg
