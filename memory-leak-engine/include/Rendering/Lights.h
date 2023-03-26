#pragma once

#include "glad/glad.h"

namespace mlg {
    struct DirectionalLight {
        glm::vec3 direction;
        float offeset1;
        glm::vec3 ambient;
        float offeset2;
        glm::vec3 diffuse;
        float offeset3;
        glm::vec3 specular;
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
