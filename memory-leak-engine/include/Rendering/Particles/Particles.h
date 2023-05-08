#pragma once

namespace mlg {
    struct ParticleProps {
        glm::vec3 position;
        glm::vec3 beginVelocity, endVelocity;
        glm::vec4 beginColor, endColor;
        glm::vec2 beginSize, endSize;
        float beginRotation, endRotation;

        float lifeTime;
    };

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
}
