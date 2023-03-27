#include "Rendering/GBuffer.h"

#include <glad/glad.h>

#include "Rendering/Assets/MaterialAsset.h"
#include "Core/AssetManager/AssetManager.h"

#include "Macros.h"

namespace mlg {
    GBuffer::GBuffer(int32_t resolutionX, int32_t resolutionY):
    screenWidth(resolutionX), screenHeight(resolutionY) {
        SPDLOG_INFO("Initializing GBuffer");

        material = AssetManager::GetAsset<MaterialAsset>("res/config/light_pass_material.json");

        InitializeFbo();
    }

    void GBuffer::InitializeFbo() {
        glGenFramebuffers(1, &gBuffer);
        GenerateAndBindTextures();

        MLG_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                       "Frame buffer is not complete");
    }

    void GBuffer::GenerateAndBindTextures() {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        glGenTextures(1, &gPositionTexture);
        glBindTexture(GL_TEXTURE_2D, gPositionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionTexture, 0);

        glGenTextures(1, &gNormalTexture);
        glBindTexture(GL_TEXTURE_2D, gNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalTexture, 0);

        glGenTextures(1, &gAlbedoSpecularTexture);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpecularTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
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

    GBuffer::~GBuffer() {
        glDeleteRenderbuffers(1, &rboDepth);
        glDeleteTextures(1, &gAlbedoSpecularTexture);
        glDeleteTextures(1, &gNormalTexture);
        glDeleteTextures(1, &gPositionTexture);
        glDeleteFramebuffers(1, &gBuffer);
    }

    void GBuffer::Activate() {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }

    void GBuffer::DeActivate() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::Draw() {
        glDisable(GL_DEPTH_TEST);

        material->Activate();

        screenSpacePlane.Activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPositionTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormalTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpecularTexture);

        screenSpacePlane.Draw();
        screenSpacePlane.DeActivate();
        material->DeActivate();

        glEnable(GL_DEPTH_TEST);
    }

    void GBuffer::CopyDepthBuffer() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::Resize(int32_t x, int32_t y) {
        //TODO: implementation
        MLG_UNIMPLEMENTED;
    }

} // mlg