#include "Rendering/FrameBuffers/SingleTextureFrameBuffer.h"

#include <glad/glad.h>

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Macros.h"

namespace mlg {
    SingleTextureFrameBuffer::SingleTextureFrameBuffer(int32_t width, int32_t height)
    : SingleTextureFrameBuffer(width, height, nullptr) {

    }

    SingleTextureFrameBuffer::SingleTextureFrameBuffer(int32_t width, int32_t height,
                                                       const std::shared_ptr<MaterialAsset>& material)
            : FrameBuffer(width, height) {
        GenerateAndBindTextures();

        MLG_ASSERT_MSG(IsFrameBufferComplete(), "Frame buffer is not complete");

        if (material == nullptr)
            this->material = AssetManager::GetAsset<MaterialAsset>("res/materials/simple_fbo_material.json");
        else
            this->material = material;
    }

    SingleTextureFrameBuffer::~SingleTextureFrameBuffer() {
        glDeleteTextures(1, &colorTexture);
    }

    void SingleTextureFrameBuffer::GenerateAndBindTextures() {
        glDeleteTextures(1, &colorTexture);

        glCreateTextures(GL_TEXTURE_2D, 1, &colorTexture);
        glTextureParameteri(colorTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(colorTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(colorTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(colorTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(colorTexture, 1, GL_RGBA8, GetWidth(), GetHeight());
        glNamedFramebufferTexture(GetFbo(), GL_COLOR_ATTACHMENT0, colorTexture, 0);
    }

    void SingleTextureFrameBuffer::Draw() {
        glBindTextureUnit(0, colorTexture);
        FrameBuffer::Draw();
    }

    void SingleTextureFrameBuffer::Resize(int32_t width, int32_t height) {
        FrameBuffer::Resize(width, height);

        GenerateAndBindTextures();
    }

    void SingleTextureFrameBuffer::Clear() {
        glClearColor(0.f, 0.f, 0.f, 0.f);
    }


} // mlg