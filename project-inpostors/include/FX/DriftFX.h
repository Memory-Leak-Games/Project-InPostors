#pragma once

#include "Rendering/Particles/ParticleSystem.h"

namespace mlg {
    class Transform;
}

class DriftFX : public mlg::ParticleSystem {
    float timeAccumulator = 0.0f;

public:
    DriftFX();

    void UpdateSystem(const class mlg::Transform& transform) override;
    void Emit(const class mlg::Transform& transform) override;

    std::shared_ptr<ParticleSystem> Clone() override;
};
