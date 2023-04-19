#pragma once

#include "glad/glad.h"

namespace mlg {
    struct DirectionalLightData {
        glm::vec3 direction;    // 0 - 16
        float offeset1;
        glm::vec3 ambient;      // 16 - 32
        float offeset2;
        glm::vec3 diffuse;      // 32 - 48
        float offeset3;
        glm::vec3 specular;     // 48 - 64
        float offeset4;

        glm::mat4 lightSpaceMatrix; // 64 - 128
    };

    class DirectionalLight {
    private:
        static DirectionalLight* instance;

        uint32_t shadowMapFrameBuffer = 0;
        uint32_t shadowMapTexture = 0;

        std::shared_ptr<class MaterialAsset> shadowMapMaterial;

        uint32_t uboLightData = 0;

        DirectionalLightData sun;

        int shadowMapResolution;
        float shadowMapSize;

        DirectionalLight();
    public:
        static DirectionalLight* GetInstance();
        virtual ~DirectionalLight();

        virtual void BindShadowMap();
        virtual void BindShadowMapShader();
        std::weak_ptr<class ShaderProgram> GetShadowShaderProgram();
        uint32_t GetShadowMap();

        [[nodiscard]] const DirectionalLightData &GetSun() const;
        void SetSun(const DirectionalLightData &sun);

        static glm::vec3 DirectionVector(float pitch, float yaw);

    private:
        void InitializeLights();
        void InitializeFrameBuffer();

        void UpdateSun();
    };
}
