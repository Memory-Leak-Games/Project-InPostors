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

    float wheelFactor = 4.f;
    glm::vec3 wheelRotation = {0.f, 0.f, 1.f};

    float donutFactor = 2.f;
    glm::vec3 donutRotation = {0.f, -1.f, 1.f};

    float jumpFactor = 1.f;
    glm::vec3 jumpRotation = {0.f, 1.f, 0.f};
    glm::vec3 jumpPosition {};

public:
    SpecialAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                              const std::weak_ptr<mlg::StaticMeshComponent>& animMesh,
                              AnimationType animType);
    ~SpecialAnimationComponent() override;

    void Start() override;
    void Update() override;

private:
    void Rotate(glm::vec3 rotationVector, float factor);
    void Jump();
};
