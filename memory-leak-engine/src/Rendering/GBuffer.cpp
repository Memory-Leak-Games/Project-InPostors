#include "Rendering/GBuffer.h"

#include <glad/glad.h>

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/RenderingAPI.h"

#include "Macros.h"

namespace mlg {
    GBuffer::GBuffer(int32_t resolutionX, int32_t resolutionY):
    screenWidth(resolutionX), screenHeight(resolutionY) {
        SPDLOG_INFO("Initializing GBuffer and SSAO");

        lightPassMaterial = AssetManager::GetAsset<MaterialAsset>("res/config/light_pass_material.json");
        ssaoPassMaterial = AssetManager::GetAsset<MaterialAsset>("res/config/ssao_material.json");
        ssaoBlurPassMaterial = AssetManager::GetAsset<MaterialAsset>("res/config/ssao_blur_material.json");

        InitializeGBuffer();
        InitializeSSAOBuffer();
    }

    void GBuffer::InitializeGBuffer() {
        glGenFramebuffers(1, &gBuffer);
        GenerateAndBindGTextures();

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        MLG_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "GBuffer is not complete");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::GenerateAndBindGTextures() {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        glGenTextures(1, &gPositionTexture);
        glBindTexture(GL_TEXTURE_2D, gPositionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionTexture, 0);

        glGenTextures(1, &gNormalTexture);
        glBindTexture(GL_TEXTURE_2D, gNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalTexture, 0);

        glGenTextures(1, &gAlbedoSpecularTexture);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpecularTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpecularTexture, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        uint32_t attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glNamedFramebufferDrawBuffers(gBuffer, 3, attachments);

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    }

    void GBuffer::InitializeSSAOBuffer() {
        glGenFramebuffers(1, &ssaoBuffer);
        GenerateAndBindSSAOTextures();
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);
        MLG_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "SSAOBuffer is not complete");

        glGenFramebuffers(1, &ssaoBlurBuffer);
        GenerateAndBindSSAOBlurTextures();
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);
        MLG_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "SSAOBlurBuffer is not complete");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::GenerateAndBindSSAOTextures() {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);
        glGenTextures(1, &ssaoTexture);
        glBindTexture(GL_TEXTURE_2D, ssaoTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);
    }

    void GBuffer::GenerateAndBindSSAOBlurTextures() {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);
        glGenTextures(1, &ssaoBlurTexture);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurTexture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GBuffer::~GBuffer() {
        glDeleteRenderbuffers(1, &rboDepth);

        glDeleteTextures(1, &gAlbedoSpecularTexture);
        glDeleteTextures(1, &gNormalTexture);
        glDeleteTextures(1, &gPositionTexture);
        glDeleteTextures(1, &ssaoTexture);
        glDeleteTextures(1, &ssaoBlurBuffer);

        glDeleteFramebuffers(1, &gBuffer);
        glDeleteFramebuffers(1, &ssaoBuffer);
        glDeleteFramebuffers(1, &ssaoBlurBuffer);
    }

    void GBuffer::Activate() {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    void GBuffer::DeActivate() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void GBuffer::DrawLightPass() {
        lightPassMaterial->Activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPositionTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormalTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpecularTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurTexture);

        RenderingAPI::GetInstance()->DrawScreenSpaceQuad();

        lightPassMaterial->DeActivate();
    }

    void GBuffer::DrawSSAOTexture() {

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);

        ssaoPassMaterial->Activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPositionTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormalTexture);

        RenderingAPI::GetInstance()->DrawScreenSpaceQuad();

        ssaoPassMaterial->DeActivate();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::DrawSSAOBlurTexture() {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);

        ssaoBlurPassMaterial->Activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoTexture);

        RenderingAPI::GetInstance()->DrawScreenSpaceQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::CopyDepthBuffer(uint32_t fbo) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void GBuffer::Resize(int32_t screenWidth, int32_t screenHeight) {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;

        GenerateAndBindGTextures();
        GenerateAndBindSSAOTextures();
        GenerateAndBindSSAOBlurTextures();
    }



} // mlg