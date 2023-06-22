#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class StaticMeshComponent;
    class Entity;
}

class SpecialAnimationComponent : public mlg::Component {
private:
    std::weak_ptr<mlg::StaticMeshComponent> animMesh;

    // Animation params
    glm::vec3 meshScale {};

    bool outputAnimOn;
    float outputAnimSpeed;
    float outputAnimWeight;

    bool workingAnimOn;
    float workingAnimSpeed;
    float workingAnimWeight;

public:
    SpecialAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                              const std::weak_ptr<mlg::StaticMeshComponent>& animMesh,
                              const std::string& configPath = "res/config/anim.json");
    ~SpecialAnimationComponent() override;

    void Start() override;
    void Update() override;

    void LoadParameters(const std::string& path);
};
