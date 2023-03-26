#pragma once

#include "glad/glad.h"

namespace mlg {
    struct DirectionalLight {
        glm::vec4 color;
        glm::vec3 direction;
    };

    class Lights {
    private:
        GLuint uboLightData;

        DirectionalLight sun;

    public:
        Lights();

        virtual ~Lights();

        [[nodiscard]] const DirectionalLight &GetSun() const;
        void SetSun(const DirectionalLight &sun);

        static glm::vec3 DirectionVector(float pitch, float yaw);

    private:
        void InitializeLights();

        void UpdateSun();
    };
}
