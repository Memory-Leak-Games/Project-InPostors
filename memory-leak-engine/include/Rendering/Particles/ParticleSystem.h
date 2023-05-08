#pragma once

#include "Rendering/LateRenderable.h"

#include "Particles.h"

namespace mlg {

    class ParticleSystem : public LateRenderable {
    private:
        std::vector<Particle> particlesPool;
        size_t poolIndex;

        std::vector<GPUParticle> particlesToRender;

        std::unique_ptr<class Rect> rect;
        std::shared_ptr<class MaterialAsset> material;

        uint32_t particlesVbo = 0;

        uint32_t maxParticlesCount;

    protected:
        explicit ParticleSystem(uint32_t maxParticlesCount);
        void SetMaterial(const std::shared_ptr<MaterialAsset>& material);

    public:
        ParticleSystem(const std::shared_ptr<MaterialAsset>& material, uint32_t maxParticlesCount);
        ~ParticleSystem();

        void Update(const class Transform& transform);

        virtual void UpdateSystem(const class Transform& transform) = 0;
        virtual GPUParticle UpdateParticle(const Particle& particle, float life);

        virtual void Emit(const ParticleProps& particleProps);

        void LateDraw(struct Renderer *renderer) override;

    private:
        void InitializeVao();

    };

} // mlg
