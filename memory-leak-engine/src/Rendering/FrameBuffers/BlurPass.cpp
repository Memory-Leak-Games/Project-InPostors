#include "Rendering/FrameBuffers/BlurPass.h"

#include <glad/glad.h>

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"

#include "Rendering/RenderingAPI.h"

#include "Macros.h"

namespace mlg {
    BlurPass::BlurPass(int32_t width, int32_t height)
    : FrameBuffer(width, height) {
        SPDLOG_DEBUG("Initializing PostProcess");

        glCreateFramebuffers(1, &horizontalBlurFrameBuffer);
        GenerateAndBindTextures();

        MLG_ASSERT_MSG(IsFrameBufferComplete(), "Frame buffer is not complete");

        material = AssetManager::GetAsset<MaterialAsset>("res/config/ssao_blur_material.json");
    }

    void BlurPass::GenerateAndBindTextures() {
        glDeleteTextures(1, &verticalBlurTexture);
        glDeleteTextures(1, &horizontalBlurTexture);

        glCreateTextures(GL_TEXTURE_2D, 1, &verticalBlurTexture);
        glTextureParameteri(verticalBlurTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(verticalBlurTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(verticalBlurTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(verticalBlurTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(verticalBlurTexture, 1, GL_RGB8, GetWidth(), GetHeight());
        glNamedFramebufferTexture(GetFbo(), GL_COLOR_ATTACHMENT0, verticalBlurTexture, 0);
        uint32_t attachment = GL_COLOR_ATTACHMENT0;
        glNamedFramebufferDrawBuffers(GetFbo(), 1, &attachment);

        glCreateTextures(GL_TEXTURE_2D, 1, &horizontalBlurTexture);
        glTextureParameteri(horizontalBlurTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(horizontalBlurTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(horizontalBlurTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(horizontalBlurTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(horizontalBlurTexture, 1, GL_RGB8, GetWidth(), GetHeight());
        glNamedFramebufferTexture(horizontalBlurFrameBuffer, GL_COLOR_ATTACHMENT0, horizontalBlurTexture, 0);
        glNamedFramebufferDrawBuffers(horizontalBlurFrameBuffer, 1, &attachment);
    }

    BlurPass::~BlurPass() {
        glDeleteFramebuffers(1, &horizontalBlurFrameBuffer);
        glDeleteTextures(1, &verticalBlurTexture);
        glDeleteTextures(1, &horizontalBlurTexture);
    }

    void BlurPass::Draw() {
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, GetFbo());

        material->Activate();
        material->GetShaderProgram()->SetFloat("radius", radius);

        // Render horizontal blur
        material->GetShaderProgram()->SetFloat("resolution", (float) GetWidth());
        material->GetShaderProgram()->SetVec2F("dir", {1.f, 0.f});
        RenderingAPI::GetInstance()->DrawScreenSpaceQuad();

        // Render vertical blur
        glBindFramebuffer(GL_FRAMEBUFFER, horizontalBlurFrameBuffer);
        glBindTextureUnit(0, verticalBlurTexture);
        material->GetShaderProgram()->SetFloat("resolution", (float) GetHeight());
        material->GetShaderProgram()->SetVec2F("dir", {0.f, 1.f});
        RenderingAPI::GetInstance()->DrawScreenSpaceQuad();

        material->DeActivate();
    }

    void BlurPass::BindTextureUnits(uint32_t color) {
        glBindTextureUnit(0, color);
    }

    uint32_t BlurPass::GetBlurredTexture() {
        return horizontalBlurTexture;
    }

    void BlurPass::Resize(int32_t width, int32_t height) {
        FrameBuffer::Resize(width, height);

        GenerateAndBindTextures();
    }

    void BlurPass::SetRadius(float radius) {
        BlurPass::radius = radius;
    }

} // mlg