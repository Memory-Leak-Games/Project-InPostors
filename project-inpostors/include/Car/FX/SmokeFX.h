#pragma once

#include "Rendering/Particles/ParticleSystem.h"

namespace mlg {
    class Transform;
}

class SmokeFX : public mlg::ParticleSystem {
public:
    SmokeFX();

    void UpdateSystem(const mlg::Transform& transform) override;
};
