#pragma once

#include "glad/glad.h"

namespace mlg {
    struct DirectionalLight {
        glm::vec3 direction;    // 0 - 16
        float offeset1;
        glm::vec3 ambient;      // 16 - 32
        float offeset2;
        glm::vec3 diffuse;      // 32 - 48
        float offeset3;
        glm::vec3 specular;     // 48 - 64

        glm::mat4 lightSpaceMatrix; // 64 - 128
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
