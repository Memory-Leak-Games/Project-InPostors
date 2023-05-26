#pragma once

#include "Rendering/Particles/ParticleSystem.h"

namespace mlg {
    class Transform;
}

class SmokeFX : public mlg::ParticleSystem {
private:
    float timeAccumulator = 0.f;
public:
    SmokeFX();

    void UpdateSystem(const mlg::Transform& transform) override;

    std::shared_ptr<ParticleSystem> Clone() override;
};
