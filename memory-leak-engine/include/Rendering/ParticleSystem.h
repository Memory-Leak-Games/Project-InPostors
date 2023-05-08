#pragma once

#include "LateRenderable.h"

namespace mlg {

    struct ParticleProps {
        glm::vec3 position;
        glm::vec3 beginVelocity, endVelocity;
        glm::vec4 beginColor, endColor;
        glm::vec2 beginSize, endSize;
        float beginRotation, endRotation;

        float lifeTime;
    };

    class ParticleSystem : public LateRenderable {
    private:
        struct Particle {
            glm::vec3 position;
            glm::vec3 beginVelocity, endVelocity;
            glm::vec4 beginColor, endColor;
            glm::vec2 beginSize, endSize;
            float beginRotation, endRotation;

            float lifeTime;
            float lifeRemaining;

            bool isActive = false;
        };

        struct GPUParticle {
            glm::vec3 position;
            glm::vec2 size;
            glm::vec4 color;
            float rotation;
        };

        std::vector<Particle> particlesPool;
        size_t poolIndex;

        std::vector<GPUParticle> particlesToRender;

        std::unique_ptr<class Rect> rect;
        std::shared_ptr<class MaterialAsset> material;

        uint32_t particlesVbo = 0;

        uint32_t maxParticlesCount;
    public:
        ParticleSystem(const std::shared_ptr<MaterialAsset>& material, uint32_t maxParticlesCount);
        ~ParticleSystem();

        virtual void OnUpdate();
        virtual void Emit(const ParticleProps& particleProps);

        void LateDraw(struct Renderer *renderer) override;

    private:
        void InitializeVao();

    };

} // mlg
