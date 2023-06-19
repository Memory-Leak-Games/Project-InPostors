#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class StaticMeshComponent;
    class Entity;
}

class AnimationComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::StaticMeshComponent> staticMeshComponent;

    // Animation params
    glm::vec3 meshScale {};
    float animSpeed;
    float wiggleWeight;

public:
    AnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                       const std::string& configPath = "res/config/anim.json");
    ~AnimationComponent() override;

    void Start() override;
    void Update() override;

    void LoadParameters(const std::string& path);
};
