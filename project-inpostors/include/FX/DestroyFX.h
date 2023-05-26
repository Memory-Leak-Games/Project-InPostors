#pragma once

#include "Rendering/Particles/ParticleSystem.h"

namespace mlg {
    class Transform;
}

class DestroyFX : public mlg::ParticleSystem {
private:
    bool firstUpdate;
public:
    DestroyFX();

    void UpdateSystem(const mlg::Transform& transform) override;

    std::shared_ptr<ParticleSystem> Clone() override;
};