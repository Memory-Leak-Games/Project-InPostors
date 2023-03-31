#include "Rendering/FrameBuffers/GBuffer.h"

#include "glad/glad.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"
#include "Rendering/RenderingAPI.h"

#include "Rendering/Camera.h"
#include "Rendering/DirectionalLight.h"

#include "Macros.h"

namespace mlg {
    GBuffer::GBuffer(int32_t width, int32_t height)
            : FrameBuffer(width, height) {
        SPDLOG_INFO("Initializing GBuffer and SSAO");
        GenerateAndBindGTextures();

        MLG_ASSERT_MSG(IsFrameBufferComplete(), "Frame buffer is not complete");

        material = AssetManager::GetAsset<MaterialAsset>("res/config/light_pass_material.json");
    }

    void GBuffer::GenerateAndBindGTextures() {
        glDeleteTextures(1, &gAlbedoSpecularTexture);
        glDeleteTextures(1, &gNormalTexture);
        glDeleteTextures(1, &gPositionTexture);

        glCreateTextures(GL_TEXTURE_2D, 1, &gPositionTexture);
        glTextureParameteri(gPositionTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(gPositionTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(gPositionTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(gPositionTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(gPositionTexture, 1, GL_RGB16F, GetWidth(), GetHeight());
        glNamedFramebufferTexture(GetFbo(), GL_COLOR_ATTACHMENT0, gPositionTexture, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &gNormalTexture);
        glTextureParameteri(gNormalTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(gNormalTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureStorage2D(gNormalTexture, 1, GL_RGB16F, GetWidth(), GetHeight());
        glNamedFramebufferTexture(GetFbo(), GL_COLOR_ATTACHMENT1, gNormalTexture, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &gAlbedoSpecularTexture);
        glTextureParameteri(gAlbedoSpecularTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(gAlbedoSpecularTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureStorage2D(gAlbedoSpecularTexture, 1, GL_RGBA16, GetWidth(), GetHeight());
        glNamedFramebufferTexture(GetFbo(), GL_COLOR_ATTACHMENT2, gAlbedoSpecularTexture, 0);

        uint32_t attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glNamedFramebufferDrawBuffers(GetFbo(), 3, attachments);

        glCreateRenderbuffers(1, &rboDepth);
        glNamedRenderbufferStorage(rboDepth, GL_DEPTH_COMPONENT, GetWidth(), GetHeight());
        glNamedFramebufferRenderbuffer(GetFbo(), GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    }

    GBuffer::~GBuffer() {
        glDeleteRenderbuffers(1, &rboDepth);

        glDeleteTextures(1, &gAlbedoSpecularTexture);
        glDeleteTextures(1, &gNormalTexture);
        glDeleteTextures(1, &gPositionTexture);
    }

    void GBuffer::Activate() {
        glBindFramebuffer(GL_FRAMEBUFFER, GetFbo());
    }

    void GBuffer::Draw() {
        material->Activate();

        glBindTextureUnit(0, gPositionTexture);
        glBindTextureUnit(1, gNormalTexture);
        glBindTextureUnit(2, gAlbedoSpecularTexture);
        glBindTextureUnit(4, DirectionalLight::GetInstance()->GetShadowMap());

        glm::mat4 lightSpace = DirectionalLight::GetInstance()->GetSun().lightSpaceMatrix;
        glm::mat4 viewToModel = glm::inverse(Camera::GetInstance()->GetCameraViewMatrix());
        glm::mat4 viewToLight = lightSpace * viewToModel;

        material->GetShaderProgram()->SetMat4F("viewToLight", viewToLight);

        RenderingAPI::GetInstance()->DrawScreenSpaceQuad();

        material->DeActivate();
    }

    void GBuffer::CopyDepthBuffer(uint32_t fbo) {
        glBlitNamedFramebuffer(GetFbo(), fbo, 0, 0, GetWidth(), GetHeight(), 0, 0, GetWidth(), GetHeight(),
                               GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void GBuffer::Resize(int32_t width, int32_t height) {
        FrameBuffer::Resize(width, height);

        GenerateAndBindGTextures();
    }

    uint32_t GBuffer::GetPositionTexture() const {
        return gPositionTexture;
    }

    uint32_t GBuffer::GetNormalTexture() const {
        return gNormalTexture;
    }

    uint32_t GBuffer::GetAlbedoTexture() const {
        return gAlbedoSpecularTexture;
    }

    void GBuffer::BindTextures(uint32_t ssao) {
        glBindTextureUnit(3, ssao);
    }

    void GBuffer::Clear() {
        FrameBuffer::Clear();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

} // mlg