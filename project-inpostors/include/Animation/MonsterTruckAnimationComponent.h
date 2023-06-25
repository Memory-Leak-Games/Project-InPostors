#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class StaticMeshComponent;
    class Entity;
}

class MonsterTruckAnimationComponent : public mlg::Component {
private:
    std::vector<std::weak_ptr<mlg::StaticMeshComponent>> animMeshes;

    // Animation params
    glm::vec3 zRotation = {1.f, 0.f, 0.f};

public:
    MonsterTruckAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                              const std::vector<std::weak_ptr<mlg::StaticMeshComponent>>& animMeshes);
    ~MonsterTruckAnimationComponent() override;

    void Start() override;
    void Update() override;

private:
    void Rotate();
};
