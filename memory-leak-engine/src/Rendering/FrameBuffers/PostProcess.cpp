#include "Rendering/FrameBuffers/PostProcess.h"

#include <glad/glad.h>

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Macros.h"

namespace mlg {

    PostProcess::PostProcess(int32_t width, int32_t height)
            : FrameBuffer(width, height) {
        SPDLOG_DEBUG("Initializing PostProcess");
        GenerateAndBindTextures();

        MLG_ASSERT_MSG(IsFrameBufferComplete(), "Frame buffer is not complete");

        material = AssetManager::GetAsset<MaterialAsset>("res/config/EngineMaterials/post_process_material.json");
    }

    void PostProcess::GenerateAndBindTextures() {
        glDeleteTextures(1, &colorTexture);
        glDeleteRenderbuffers(1, &rboDepth);

        glCreateTextures(GL_TEXTURE_2D, 1, &colorTexture);
        glTextureParameteri(colorTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(colorTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(colorTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(colorTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(colorTexture, 1, GL_RGB8, GetWidth(), GetHeight());
        glNamedFramebufferTexture(GetFbo(), GL_COLOR_ATTACHMENT0, colorTexture, 0);

        glCreateRenderbuffers(1, &rboDepth);
        glNamedRenderbufferStorage(rboDepth, GL_DEPTH_COMPONENT, GetWidth(), GetHeight());
        glNamedFramebufferRenderbuffer(GetFbo(), GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    }

    void PostProcess::Resize(int32_t width, int32_t height) {
        FrameBuffer::Resize(width, height);

        GenerateAndBindTextures();
    }

    PostProcess::~PostProcess() {
        glDeleteRenderbuffers(1, &rboDepth);
        glDeleteTextures(1, &colorTexture);
    }

    void PostProcess::CopyDepthBuffer(uint32_t fbo) {
        glBlitNamedFramebuffer(GetFbo(), fbo, 0, 0, GetWidth(), GetHeight(), 0, 0, GetWidth(), GetHeight(),
                               GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void PostProcess::Draw() {
        glBindTextureUnit(0, colorTexture);
        FrameBuffer::Draw();
    }
} // mlg