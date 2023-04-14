#pragma once

#include "FrameBuffer.h"

namespace mlg {

    class SingleTextureFrameBuffer : public FrameBuffer {
    private:
        uint32_t colorTexture = 0;
    public:
        SingleTextureFrameBuffer(int32_t width, int32_t height);
        ~SingleTextureFrameBuffer() override;

        void Draw() override;

        void Clear() override;

        void Resize(int32_t width, int32_t height) override;

    private:
        void GenerateAndBindTextures();
    };

} // mlg
