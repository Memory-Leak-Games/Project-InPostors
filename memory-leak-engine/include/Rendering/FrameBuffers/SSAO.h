#pragma once

#include "Rendering/FrameBuffers/FrameBuffer.h"

namespace mlg {

    class SSAO : public FrameBuffer {
    private:
        uint32_t strengthTexture = 0;

    public:
        SSAO() = delete;

        SSAO(int32_t width, int32_t height);
        ~SSAO() override;

        void Draw() override;

        void BindTextureUnits(uint32_t position, uint32_t normal);
        void Resize(int32_t width, int32_t height) override;
        uint32_t GetOutput();

    private:
        void GenerateAndBindTextures() ;
    };

} // mlg
