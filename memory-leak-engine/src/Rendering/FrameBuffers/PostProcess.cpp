#include "Rendering/FrameBuffers/PostProcess.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/RenderingAPI.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Macros.h"

namespace mlg {


    PostProcess::PostProcess(int32_t width, int32_t height)
            : width(width), height(height) {
        SPDLOG_DEBUG("Initializing FrameBuffer");

        InitializeFbo();

        material = AssetManager::GetAsset<MaterialAsset>("res/config/post_process_material.json");
    }

    void PostProcess::InitializeFbo() {
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        GenerateAndBindTextures();

        MLG_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                       "Frame buffer is not complete");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::GenerateAndBindTextures() {
        glDeleteTextures(1, &colorTexture);
        glDeleteRenderbuffers(1, &rboDepth);

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::Resize(int32_t width, int32_t height) {
        this->width = width;
        this->height = height;

        GenerateAndBindTextures();
    }

    void PostProcess::Activate() {
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

    void PostProcess::CopyDepthBuffer(uint32_t fbo) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }
} // mlg