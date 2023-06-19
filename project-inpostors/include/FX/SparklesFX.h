#pragma once

#include "Rendering/Particles/ParticleSystem.h"

namespace mlg {
    class Transform;
}

class SparklesFX : public mlg::ParticleSystem {
public:
    SparklesFX();

    void UpdateSystem(const mlg::Transform& transform) override;

    std::shared_ptr<ParticleSystem> Clone() override;
};