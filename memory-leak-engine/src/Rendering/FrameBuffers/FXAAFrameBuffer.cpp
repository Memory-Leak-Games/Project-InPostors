#include "Rendering/FrameBuffers/FXAAFrameBuffer.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

namespace mlg {
    FXAAFrameBuffer::FXAAFrameBuffer(int32_t width, int32_t height)
    : SingleTextureFrameBuffer(width, height) {
        material = AssetManager::GetAsset<MaterialAsset>("res/config/EngineMaterials/fxaa_material.json");

        glCreateRenderbuffers(1, &rboDepth);
        glNamedRenderbufferStorage(rboDepth, GL_DEPTH_COMPONENT, GetWidth(), GetHeight());
        glNamedFramebufferRenderbuffer(GetFbo(), GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        MLG_ASSERT_MSG(IsFrameBufferComplete(), "Frame buffer is not complete");
    }
} // mlg