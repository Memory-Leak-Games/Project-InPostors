#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class StaticMeshComponent;
    class Entity;
}

class StorageAnimationComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::StaticMeshComponent> staticMeshComponent;

    std::shared_ptr<class Storage> storage;

    // Animation params
    glm::vec3 meshScale {};

    bool animate = false;
    float time = 0.f;

    float storageSpeed ;
    float storageAmplitude;

public:
    StorageAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                              const std::string& configPath = "res/config/anim.json");
    ~StorageAnimationComponent() override;

    void Start() override;
    void Update() override;

    void LoadParameters(const std::string& path);
};
