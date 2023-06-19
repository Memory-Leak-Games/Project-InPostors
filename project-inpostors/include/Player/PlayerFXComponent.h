#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class RigidbodyComponent;
    class ParticleSystemComponent;
}

class PlayerFXComponent : public mlg::Component {
private:
    std::weak_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::weak_ptr<mlg::ParticleSystemComponent> smokeParticleSystem;

public:
    PlayerFXComponent(const std::weak_ptr<mlg::Entity>& owner, const std::string& name);
    ~PlayerFXComponent() override;

    void Start() override;
    void Update() override;

private:
    void OnTaskFinished();
};