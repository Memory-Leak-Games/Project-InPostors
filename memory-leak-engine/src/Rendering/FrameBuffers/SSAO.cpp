#include "Rendering/FrameBuffers/SSAO.h"

#include <glad/glad.h>

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Macros.h"

namespace mlg {
    SSAO::~SSAO() {
        glDeleteTextures(1, &strengthTexture);
    }

    SSAO::SSAO(int32_t width, int32_t height)
    : FrameBuffer(width, height) {
        SPDLOG_DEBUG("Initializing SSAO");
        GenerateAndBindTextures();

        MLG_ASSERT_MSG(IsFrameBufferComplete(), "Frame buffer is not complete");

        material = AssetManager::GetAsset<MaterialAsset>("res/config/ssao_material.json");
    }

    void SSAO::GenerateAndBindTextures() {
        glDeleteTextures(1, &strengthTexture);

        glCreateTextures(GL_TEXTURE_2D, 1, &strengthTexture);
        glTextureParameteri(strengthTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(strengthTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(strengthTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(strengthTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(strengthTexture, 1, GL_R8, GetWidth(), GetHeight());
        glNamedFramebufferTexture(GetFbo(), GL_COLOR_ATTACHMENT0, strengthTexture, 0);
    }

    void SSAO::Resize(int32_t width, int32_t height) {
        FrameBuffer::Resize(width, height);

        GenerateAndBindTextures();
    }

    void SSAO::BindTextureUnits(uint32_t position, uint32_t normal) {
        glBindTextureUnit(0, position);
        glBindTextureUnit(1, normal);
    }

    uint32_t SSAO::GetOutput() {
        return strengthTexture;
    }

    void SSAO::Draw() {
        glBindFramebuffer(GL_FRAMEBUFFER, GetFbo());
        FrameBuffer::Draw();
    }

} // mlg