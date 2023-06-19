#pragma once

#include "Rendering/Particles/ParticleSystem.h"

namespace mlg {
    class Transform;
}

class CashFX : public mlg::ParticleSystem {
public:
    CashFX();

    void UpdateSystem(const mlg::Transform& transform) override;

    std::shared_ptr<ParticleSystem> Clone() override;
};