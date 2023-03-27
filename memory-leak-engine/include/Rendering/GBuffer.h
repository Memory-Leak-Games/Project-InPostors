#pragma once

#include "QuadRenderer.h"

namespace mlg {

    class GBuffer {
    private:
        uint32_t gBuffer;

        uint32_t gPositionTexture;
        uint32_t gNormalTexture;
        uint32_t gAlbedoSpecularTexture;

        uint32_t rboDepth;

        std::shared_ptr<class MaterialAsset> material;
        QuadRenderer screenSpacePlane;

        int32_t screenWidth;
        int32_t screenHeight;

        GBuffer() = default;
    public:
        GBuffer(int32_t resolutionX, int32_t resolutionY);
        ~GBuffer();

        virtual void Activate();
        virtual void DeActivate();

        virtual void Draw();
        virtual void CopyDepthBuffer();

        virtual void Resize(int32_t x, int32_t y);

    private:
        void InitializeFbo();
        void GenerateAndBindTextures() ;

    };

} // mlg
