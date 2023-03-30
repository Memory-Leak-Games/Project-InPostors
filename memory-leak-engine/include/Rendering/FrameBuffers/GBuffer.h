#pragma once

namespace mlg {

    class GBuffer {
    private:
        uint32_t gBuffer = 0;

        uint32_t gPositionTexture = 0;
        uint32_t gNormalTexture = 0;
        uint32_t gAlbedoSpecularTexture = 0;

        uint32_t rboDepth = 0;

        std::shared_ptr<class MaterialAsset> lightPassMaterial;

        int32_t screenWidth;
        int32_t screenHeight;

        GBuffer() = default;
    public:
        GBuffer(int32_t resolutionX, int32_t resolutionY);
        ~GBuffer();

        virtual void Activate();

        void BindTextures(uint32_t ssao);
        virtual void DrawLightPass();

        virtual void CopyDepthBuffer(uint32_t fbo);

        virtual void Resize(int32_t screenWidth, int32_t screenHeight);

    public:
        [[nodiscard]] const uint32_t GetPositionTexture() const;
        [[nodiscard]] const uint32_t GetNormalTexture() const;
        [[nodiscard]] const uint32_t GetAlbedoTexture() const;

    private:
        void InitializeGBuffer();
        void GenerateAndBindGTextures();

    };

} // mlg
