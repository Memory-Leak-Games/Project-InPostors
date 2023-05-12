#pragma once

#include "SingleTextureFrameBuffer.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

namespace mlg {

    class FXAAFrameBuffer : public SingleTextureFrameBuffer {
    private:
        uint32_t rboDepth = 0;
    public:
        FXAAFrameBuffer(int32_t width, int32_t height);

        void Resize(int32_t width, int32_t height) override;
    };

} // mlg
