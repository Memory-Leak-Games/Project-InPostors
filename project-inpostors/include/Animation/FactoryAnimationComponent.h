#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class StaticMeshComponent;
    class Entity;
}

class FactoryAnimationComponent : public mlg::Component {
private:
    std::shared_ptr<mlg::StaticMeshComponent> staticMeshComponent;

    std::shared_ptr<class Factory> factory;

    // Animation params
    glm::vec3 meshScale {};

    bool outputAnimOn;
    float outputAnimSpeed;
    float outputAnimWeight;

    bool workingAnimOn;
    float workingAnimSpeed;
    float workingAnimWeight;

public:
    FactoryAnimationComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name,
                       const std::string& configPath = "res/config/anim.json");
    ~FactoryAnimationComponent() override;

    void Start() override;
    void Update() override;

    void LoadParameters(const std::string& path);
};
