#pragma once

#include "Gameplay/SceneComponent.h"
#include "Rendering/Renderable.h"
#include "Rendering/Model.h"

namespace mlg {

    class StaticMeshComponent : public SceneComponent, public Renderable {
    private:
        std::shared_ptr<Model> model;
    public:
        StaticMeshComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                            const std::shared_ptr<Model>& model);

        void Start() override;

        void Draw(struct Renderer* renderer) override;

        ~StaticMeshComponent() override = default;
    };

} // mlg
