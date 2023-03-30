#include "Rendering/FrameBuffers/FrameBuffer.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/RenderingAPI.h"

#include <glad/glad.h>

#include "Macros.h"

namespace mlg {
    FrameBuffer::FrameBuffer(int32_t width, int32_t height)
    : width(width), height(height) {
        glCreateFramebuffers(1, &fbo);
    }

    uint32_t FrameBuffer::GetFbo() {
        return fbo;
    }

    bool FrameBuffer::IsFrameBufferComplete() {
        return glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    void FrameBuffer::Activate() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void FrameBuffer::Draw() {
        glDisable(GL_DEPTH_TEST);
        material->Activate();
        RenderingAPI::GetInstance()->DrawScreenSpaceQuad();
        material->DeActivate();
    }

    void FrameBuffer::Resize(int32_t width, int32_t height) {
        this->width = width;
        this->height = height;
    }

    int32_t FrameBuffer::GetWidth() {
        return width;
    }

    int32_t FrameBuffer::GetHeight() {
        return height;
    }

    FrameBuffer::~FrameBuffer() {
        uint32_t fbo = GetFbo();
        glDeleteFramebuffers(1, &fbo);
    }

} // mlg