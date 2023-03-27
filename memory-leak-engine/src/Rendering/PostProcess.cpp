#include "Rendering/PostProcess.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Macros.h"

namespace mlg {


    PostProcess::PostProcess(int32_t resolutionX, int32_t resolutionY)
            : frameBuffer(0), colorTexture(0), depthStencilTexture(0) {
        SPDLOG_DEBUG("Initializing FrameBuffer");

        InitializeFbo(resolutionX, resolutionY);

        material = AssetManager::GetAsset<MaterialAsset>("res/config/post_process_material.json");
    }

    void PostProcess::InitializeFbo(int32_t resolutionX, int32_t resolutionY) {
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        GenerateAndBindTextures(resolutionX, resolutionY);

        MLG_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                       "Frame buffer is not complete");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::GenerateAndBindTextures(int32_t resolutionX, int32_t resolutionY) {
        glDeleteTextures(1, &colorTexture);
        glDeleteTextures(1, &depthStencilTexture);

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolutionX, resolutionY, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

        glGenTextures(1, &depthStencilTexture);
        glBindTexture(GL_TEXTURE_2D, depthStencilTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolutionX, resolutionY, 0, GL_DEPTH_STENCIL,
                     GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::Resize(int32_t x, int32_t y) {
        GenerateAndBindTextures(x, y);
    }

    void PostProcess::Activate() {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glEnable(GL_DEPTH_TEST);
    }

    void PostProcess::Clear(glm::vec4 color) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void PostProcess::DeActivate() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::Draw() {
        glDisable(GL_DEPTH_TEST);

        material->Activate();

        screenSpacePlane.Activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D, colorTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthStencilTexture);

        screenSpacePlane.Draw();
        screenSpacePlane.DeActivate();
        material->DeActivate();
    }

    PostProcess::~PostProcess() {
        glDeleteTextures(1, &colorTexture);
        glDeleteTextures(1, &depthStencilTexture);
        glDeleteFramebuffers(1, &frameBuffer);
    }
} // mlg