#pragma once

#include "glm/glm.hpp"
#include "glad/glad.h"

namespace mlg {
    struct DirectionalLight {
        glm::vec4 color;
        glm::vec3 direction;
    };

    struct PointLight {
        glm::vec4 color;
        glm::vec3 position;
        float linear;
        float quadratic;
    };

    struct SpotLight {
        glm::vec4 color;
        glm::vec3 position;
        float _GAP;
        glm::vec3 direction;
        float linear;
        float quadratic;
        float cutOff;
        float outerCutOff;
    };

    class Lights {
    private:
        GLuint uboLightData;

        DirectionalLight sun;
        PointLight bulb;
        SpotLight spotLights[2];

    public:
        Lights();

        virtual ~Lights();

        void DrawGizmos();

        [[nodiscard]] const DirectionalLight &GetSun() const;

        [[nodiscard]] const PointLight &GetBulb() const;

        [[nodiscard]] const SpotLight &GetSpotLightOne() const;

        [[nodiscard]] const SpotLight &GetSpotLightTwo() const;

        void SetSun(const DirectionalLight &sun);

        void SetBulb(const PointLight &bulb);

        void SetSpotLightOne(const SpotLight &spotLightOne);

        void SetSpotLightTwo(const SpotLight &spotLightTwo);

        static glm::vec3 DirectionVector(float pitch, float yaw);

    private:
        void InitializeLights();

        void UpdateSun();

        void UpdateBulb();

        void UpdateSpotLights();

        void UpdateSpotLightTwo();
    };
}
