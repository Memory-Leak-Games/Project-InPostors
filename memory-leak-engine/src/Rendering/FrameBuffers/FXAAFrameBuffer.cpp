#include "Rendering/FrameBuffers/FXAAFrameBuffer.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

namespace mlg {
    FXAAFrameBuffer::FXAAFrameBuffer(int32_t width, int32_t height)
    : SingleTextureFrameBuffer(width, height) {
        material = AssetManager::GetAsset<MaterialAsset>("res/config/EngineMaterials/fxaa_material.json");
    }
} // mlg