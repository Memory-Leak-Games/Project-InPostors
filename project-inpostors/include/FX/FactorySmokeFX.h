#pragma once

#include "Rendering/Particles/ParticleSystem.h"

class FactorySmokeFX : public mlg::ParticleSystem {
public:
    FactorySmokeFX();

    void UpdateSystem(const mlg::Transform& transform) override;

    std::shared_ptr<ParticleSystem> Clone() override;
};
