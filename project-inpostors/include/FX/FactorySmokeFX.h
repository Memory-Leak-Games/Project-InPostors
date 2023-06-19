#pragma once

#include "Rendering/Particles/ParticleSystem.h"

class FactorySmokeFX : public mlg::ParticleSystem {
private:
    float timeAccumulator = 0.f;
    float timeToSpawn = 0.1f;
public:
    FactorySmokeFX();

    void SetTimeToSpawn(float timeToSpawn);

    void UpdateSystem(const mlg::Transform& transform) override;

    std::shared_ptr<ParticleSystem> Clone() override;
};
