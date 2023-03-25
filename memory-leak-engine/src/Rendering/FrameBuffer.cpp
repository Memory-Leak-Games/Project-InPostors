#include "Rendering/FrameBuffer.h"

#include "Core/AssetManager/AssetManager.h"
#include "Macros.h"

namespace mlg {


    FrameBuffer::FrameBuffer(int32_t resolutionX, int32_t resolutionY)
            : vao(0), vbo(0), frameBuffer(0), colorTexture(0), depthStencilTexture(0) {
        SPDLOG_DEBUG("Initializing FrameBuffer");

        InitializeVao();
        InitializeFbo(resolutionX, resolutionY);

        material = AssetManager::GetAsset<MaterialAsset>("res/config/post_process_material.json");
    }

    void FrameBuffer::InitializeVao() {
        // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        static const std::vector<float> quadVertices({
                                                             // positions   // texCoords
                                                             -1.0f, 1.0f, 0.0f, 1.0f,
                                                             -1.0f, -1.0f, 0.0f, 0.0f,
                                                             1.0f, -1.0f, 1.0f, 0.0f,

                                                             -1.0f, 1.0f, 0.0f, 1.0f,
                                                             1.0f, -1.0f, 1.0f, 0.0f,
                                                             1.0f, 1.0f, 1.0f, 1.0f
                                                     });

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, (int32_t) (quadVertices.size() * sizeof(float)), &quadVertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));

        glBindVertexArray(0);
    }

    void FrameBuffer::InitializeFbo(int32_t resolutionX, int32_t resolutionY) {
        glGenFramebuffers(1, &frameBuffer);
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

        MLG_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                       "Frame buffer is not complete");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Resize(int32_t x, int32_t y) {
        //TODO: Resize Implementation
        MLG_UNIMPLEMENTED;
    }

    void FrameBuffer::Activate() {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glEnable(GL_DEPTH_TEST);
    }

    void FrameBuffer::Clear(glm::vec4 color) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void FrameBuffer::DeActivate() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Draw() {
        glDisable(GL_DEPTH_TEST);

        material->Activate();

        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);
        material->GetShaderProgram()->SetInt("ColorTexture", 0);
        glBindTexture(GL_TEXTURE_2D, colorTexture);

        glActiveTexture(GL_TEXTURE1);
        material->GetShaderProgram()->SetInt("DepthStencilTexture", 1);
        glBindTexture(GL_TEXTURE_2D, depthStencilTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        material->DeActivate();
    }

    FrameBuffer::~FrameBuffer() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glDeleteTextures(1, &colorTexture);
        glDeleteTextures(1, &depthStencilTexture);
        glDeleteFramebuffers(1, &frameBuffer);
    }
} // mlg