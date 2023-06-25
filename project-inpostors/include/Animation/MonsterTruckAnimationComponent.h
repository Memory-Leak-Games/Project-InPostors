#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class StaticMeshComponent;
    class RigidbodyComponent;
    class Entity;
}

class MonsterTruckAnimationComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::vector<std::weak_ptr<mlg::StaticMeshComponent>> animMeshes;

    // Animation params
    glm::vec3 animRotation = {1.f, 0.f, 0.f};

public:
    MonsterTruckAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                              const std::vector<std::weak_ptr<mlg::StaticMeshComponent>>& animMeshes);
    ~MonsterTruckAnimationComponent() override;

    void Start() override;
    void Update() override;

private:
    void Rotate();
};
