#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class StaticMeshComponent;
    class Entity;
}

enum AnimationType {
    rotateZ,
    rotateYZ,
    jump
};

class SpecialAnimationComponent : public mlg::Component {
private:
    std::weak_ptr<mlg::StaticMeshComponent> animMesh;

    // Animation params
    AnimationType animType;
    glm::vec3 zRotation = {0.f, 0.f, 1.f};
    glm::vec3 yzRotation = {0.f, -1.f, 1.f};

    bool outputAnimOn;
    float outputAnimSpeed;
    float outputAnimWeight;

    bool workingAnimOn;
    float workingAnimSpeed;
    float workingAnimWeight;

public:
    SpecialAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                              const std::weak_ptr<mlg::StaticMeshComponent>& animMesh,
                              AnimationType animType,
                              const std::string& configPath = "res/config/anim.json");
    ~SpecialAnimationComponent() override;

    void Start() override;
    void Update() override;

    void LoadParameters(const std::string& path);

private:
    void RotateZ();
    void RotateYZ();
};
