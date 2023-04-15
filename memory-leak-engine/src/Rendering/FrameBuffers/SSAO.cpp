#include "Rendering/FrameBuffers/SSAOFrameBuffer.h"

#include <glad/glad.h>

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Macros.h"

namespace mlg {
    SSAOFrameBuffer::~SSAOFrameBuffer() {
        glDeleteTextures(1, &strengthTexture);
    }

    SSAOFrameBuffer::SSAOFrameBuffer(int32_t width, int32_t height)
    : FrameBuffer(width, height) {
        SPDLOG_DEBUG("Initializing SSAO");
        GenerateAndBindTextures();

        MLG_ASSERT_MSG(IsFrameBufferComplete(), "Frame buffer is not complete");

        material = AssetManager::GetAsset<MaterialAsset>("res/config/EngineMaterials/ssao_material.json");
    }

    void SSAOFrameBuffer::GenerateAndBindTextures() {
        glDeleteTextures(1, &strengthTexture);

        glCreateTextures(GL_TEXTURE_2D, 1, &strengthTexture);
        glTextureParameteri(strengthTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(strengthTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(strengthTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(strengthTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(strengthTexture, 1, GL_R8, GetWidth(), GetHeight());
        glNamedFramebufferTexture(GetFbo(), GL_COLOR_ATTACHMENT0, strengthTexture, 0);
    }

    void SSAOFrameBuffer::Resize(int32_t width, int32_t height) {
        FrameBuffer::Resize(width, height);

        GenerateAndBindTextures();
    }

    void SSAOFrameBuffer::BindTextureUnits(uint32_t position, uint32_t normal) {
        glBindTextureUnit(0, position);
        glBindTextureUnit(1, normal);
    }

    uint32_t SSAOFrameBuffer::GetOutput() {
        return strengthTexture;
    }

    void SSAOFrameBuffer::Draw() {
        glBindFramebuffer(GL_FRAMEBUFFER, GetFbo());
        FrameBuffer::Draw();
    }

} // mlg