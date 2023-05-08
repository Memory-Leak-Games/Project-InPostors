#pragma once

#include "Rendering/FrameBuffers/FrameBuffer.h"

namespace mlg {

    class PostProcess : public FrameBuffer {
    private:
        uint32_t colorTexture = 0;
        uint32_t rboDepth = 0;

    public:
        PostProcess() = delete;

        PostProcess(int32_t width, int32_t height);
        ~PostProcess() override;

        void Draw() override;

        void Resize(int32_t width, int32_t height) override;

    private:
        void GenerateAndBindTextures() ;
    };

} // mlg
