#pragma once

#include "QuadRenderer.h"

namespace mlg {

    class GBuffer {
    private:
        uint32_t gBuffer = 0;
        uint32_t ssaoBuffer = 0;
        uint32_t ssaoBlurBuffer = 0;

        uint32_t gPositionTexture = 0;
        uint32_t gNormalTexture = 0;
        uint32_t gAlbedoSpecularTexture = 0;

        uint32_t ssaoTexture = 0;
        uint32_t ssaoBlurTexture = 0;

        uint32_t rboDepth = 0;

        std::shared_ptr<class MaterialAsset> lightPassMaterial;
        std::shared_ptr<MaterialAsset> ssaoPassMaterial;
        std::shared_ptr<MaterialAsset> ssaoBlurPassMaterial;

        QuadRenderer screenSpacePlane;

        int32_t screenWidth;
        int32_t screenHeight;

        GBuffer() = default;
    public:
        GBuffer(int32_t resolutionX, int32_t resolutionY);
        ~GBuffer();

        virtual void Activate();
        virtual void DeActivate();

        virtual void DrawSSAOTexture();
        virtual void DrawSSAOBlurTexture();
        virtual void DrawLightPass();

        virtual void CopyDepthBuffer();

        virtual void Resize(int32_t screenWidth, int32_t screenHeight);

    private:
        void InitializeGBuffer();
        void GenerateAndBindGTextures();

        void InitializeSSAOBuffer();
        void GenerateAndBindSSAOTextures();
        void GenerateAndBindSSAOBlurTextures();


    };

} // mlg
