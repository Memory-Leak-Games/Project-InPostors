#pragma once

#include "Rendering/FrameBuffers/FrameBuffer.h"

namespace mlg {

    class SSAOFrameBuffer : public FrameBuffer {
    private:
        uint32_t strengthTexture = 0;
        uint32_t numberOfSamples = 16;

    public:
        SSAOFrameBuffer() = delete;

        SSAOFrameBuffer(int32_t width, int32_t height);
        ~SSAOFrameBuffer() override;

        void Draw() override;

        void BindTextureUnits(uint32_t position, uint32_t normal);
        void Resize(int32_t width, int32_t height) override;
        uint32_t GetOutput();

    private:
        void GenerateAndBindTextures() ;

        void GenerateAndSendSamples();
        void GenerateAndSendNoise();
    };

} // mlg
