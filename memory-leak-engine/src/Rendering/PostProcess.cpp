#include "Rendering/PostProcess.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/RenderingAPI.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Macros.h"

namespace mlg {


    PostProcess::PostProcess(int32_t resolutionX, int32_t resolutionY)
            : frameBuffer(0), colorTexture(0), rboDepth(0) {
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

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolutionX, resolutionY, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolutionX, resolutionY);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::Resize(int32_t x, int32_t y) {
        GenerateAndBindTextures(x, y);
    }

    void PostProcess::Activate() {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    }

    void PostProcess::Clear(glm::vec4 color) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    }

    void PostProcess::DeActivate() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::Draw() {
        glDisable(GL_DEPTH_TEST);

        material->Activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D, colorTexture);

        RenderingAPI::GetInstance()->DrawScreenSpaceQuad();

        material->DeActivate();
    }

    PostProcess::~PostProcess() {
        glDeleteRenderbuffers(1, &rboDepth);
        glDeleteTextures(1, &colorTexture);
        glDeleteFramebuffers(1, &frameBuffer);
    }

    uint32_t PostProcess::GetFrameBuffer() const {
        return frameBuffer;
    }
} // mlg