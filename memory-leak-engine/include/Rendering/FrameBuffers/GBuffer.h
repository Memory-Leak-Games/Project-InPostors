#pragma once

#include "FrameBuffer.h"

namespace mlg {

    class GBuffer : public FrameBuffer {
    private:
        uint32_t gPositionTexture = 0;
        uint32_t gNormalTexture = 0;
        uint32_t gAlbedoSpecularTexture = 0;

        uint32_t rboDepth = 0;

        bool isSSAOActive = false;

        GBuffer() = default;
    public:
        GBuffer(int32_t width, int32_t height);
        ~GBuffer() override;

        void Clear() override;
        void Activate() override;

        void BindTextures(uint32_t ssao);
        void Draw() override;

        virtual void CopyDepthBuffer(uint32_t fbo);

        void Resize(int32_t width, int32_t height) override;

    public:
        [[nodiscard]] uint32_t GetPositionTexture() const;
        [[nodiscard]] uint32_t GetNormalTexture() const;
        [[nodiscard]] uint32_t GetAlbedoTexture() const;

    private:
        void GenerateAndBindGTextures();
    };

} // mlg
