#pragma once

#include "SingleTextureFrameBuffer.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

namespace mlg {

    class FXAAFrameBuffer : public SingleTextureFrameBuffer {
    public:
        FXAAFrameBuffer(int32_t width, int32_t height);
    };

} // mlg