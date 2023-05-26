#pragma once

#include "Rendering/Particles/ParticleSystem.h"

class FactorySmokeFX : public mlg::ParticleSystem {
private:
    float timeAccumulator = 0.f;
public:
    FactorySmokeFX();

    void UpdateSystem(const mlg::Transform& transform) override;

    std::shared_ptr<ParticleSystem> Clone() override;
};
