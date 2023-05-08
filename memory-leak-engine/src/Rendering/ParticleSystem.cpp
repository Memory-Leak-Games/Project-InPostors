#include "Rendering/ParticleSystem.h"
#include "Rendering/Primitives/Rect.h"

#include "Rendering/Renderer.h"

#include "Rendering/Assets/MaterialAsset.h"

#include "Core/Math.h"
#include "Core/Time.h"

namespace mlg {
    ParticleSystem::ParticleSystem(const std::shared_ptr<MaterialAsset>& material, uint32_t maxParticlesCount)
    : maxParticlesCount(maxParticlesCount), poolIndex(maxParticlesCount - 1) {
        particlesPool.resize(maxParticlesCount);
        rect = std::make_unique<Rect>();

        this->material = material;

        InitializeVao();
    }

    ParticleSystem::~ParticleSystem() {
        glDeleteBuffers(1, &particlesVbo);
    }

    void ParticleSystem::InitializeVao() {
        uint32_t rectVao = rect->GetVao();

        glCreateBuffers(1, &particlesVbo);

        glNamedBufferData(particlesVbo, maxParticlesCount * sizeof(GPUParticle), nullptr, GL_DYNAMIC_DRAW);

        // position
        glEnableVertexArrayAttrib(rectVao, 3);
        glVertexArrayAttribBinding(rectVao, 3, 1);
        glVertexArrayAttribFormat(rectVao, 3, 3, GL_FLOAT, GL_FALSE, offsetof(GPUParticle, position));

        // size
        glEnableVertexArrayAttrib(rectVao, 4);
        glVertexArrayAttribBinding(rectVao, 4, 1);
        glVertexArrayAttribFormat(rectVao, 4, 2, GL_FLOAT, GL_FALSE, offsetof(GPUParticle, size));

        // color
        glEnableVertexArrayAttrib(rectVao, 5);
        glVertexArrayAttribBinding(rectVao, 5, 1);
        glVertexArrayAttribFormat(rectVao, 5, 4, GL_FLOAT, GL_TRUE, offsetof(GPUParticle, color));

        // rotation
        glEnableVertexArrayAttrib(rectVao, 6);
        glVertexArrayAttribBinding(rectVao, 6, 1);
        glVertexArrayAttribFormat(rectVao, 6, 1, GL_FLOAT, GL_FALSE, offsetof(GPUParticle, rotation));

        glVertexArrayBindingDivisor(rectVao, 1, 1);
        glVertexArrayVertexBuffer(rectVao, 1, particlesVbo, 0, sizeof(GPUParticle));
    }

    void ParticleSystem::LateDraw(struct Renderer *renderer) {
        if (particlesToRender.empty())
            return;

        glNamedBufferSubData(particlesVbo, 0, particlesToRender.size() * sizeof(GPUParticle), &particlesToRender.front());

        material->Activate();

        glBindVertexArray(rect->GetVao());
        glDrawArraysInstanced(GL_TRIANGLES, 0, rect->GetSize(), particlesToRender.size());
    }

    void ParticleSystem::OnUpdate() {
        particlesToRender.clear();

        float deltaSeconds = Time::GetDeltaSeconds();

        for (auto& particle : particlesPool) {
            if (!particle.isActive)
                continue;

            if (particle.lifeRemaining <= 0.f) {
                particle.isActive = false;
                continue;
            }

            particle.lifeRemaining -= deltaSeconds;

            float life = 1 - particle.lifeRemaining / particle.lifeTime;

            glm::vec3 velocity = Math::Lerp(particle.beginVelocity, particle.endVelocity, life);

            glm::vec3 position = particle.position + velocity * (particle.lifeTime - particle.lifeRemaining);
            glm::vec4 color = Math::Lerp(particle.beginColor, particle.endColor, life);
            glm::vec2 size = Math::Lerp(particle.beginSize, particle.endSize, life);
            float rotation = Math::Lerp(particle.beginRotation, particle.endRotation, life);

            GPUParticle gpuParticle {position, size, color, rotation};
            particlesToRender.push_back(gpuParticle);
        }
    }

    void ParticleSystem::Emit(const ParticleProps& particleProps) {
        Particle& particle = particlesPool[poolIndex];
        particle.isActive = true;

        particle.position = particleProps.position;
        particle.beginVelocity = particleProps.beginVelocity;
        particle.endVelocity = particleProps.endVelocity;

        particle.beginColor = particleProps.beginColor;
        particle.endColor = particleProps.endColor;

        particle.beginSize = particleProps.beginSize;
        particle.endSize = particleProps.endSize;

        particle.beginRotation = particleProps.beginRotation;
        particle.endRotation = particleProps.endRotation;

        particle.lifeTime = particleProps.lifeTime;
        particle.lifeRemaining = particleProps.lifeTime;

        poolIndex = --poolIndex % maxParticlesCount;
    }

} // mlg