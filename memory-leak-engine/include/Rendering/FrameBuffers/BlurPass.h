#pragma once

#include "FrameBuffer.h"

namespace mlg {

    class BlurPass : public FrameBuffer {
    private:
        uint32_t horizontalBlurFrameBuffer = 0;

        uint32_t verticalBlurTexture = 0;
        uint32_t horizontalBlurTexture = 0;

        float radius = 1.f;
    public:
        BlurPass() = delete;

        BlurPass(int32_t width, int32_t height);
        ~BlurPass() override;

        void BindTextureUnits(uint32_t color);
        void Draw() override;

        void Resize(int32_t width, int32_t height) override;

        uint32_t GetBlurredTexture();

        void SetRadius(float radius);

    private:
        void GenerateAndBindTextures();
    };

} // mlg
