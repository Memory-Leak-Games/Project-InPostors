#include "Rendering/DirectionalLight.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"
#include "Core/AssetManager/AssetManager.h"

using namespace mlg;

DirectionalLight::DirectionalLight() {
    InitializeLights();

    glCreateBuffers(1, &uboLightData);
    glNamedBufferData(uboLightData, sizeof(DirectionalLightData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLightData);

    InitializeFrameBuffer();

    UpdateSun();
}

void DirectionalLight::InitializeLights() {
    sun.direction = glm::normalize(glm::vec3(0.5f, -0.5f, 0.5f));
    sun.diffuse = glm::vec3(0.7f);
    sun.ambient = glm::vec3(0.3f);
    sun.specular = glm::vec3(0.1f);

    glm::vec3 position{-20.f, 20.f, -20.f};

    glm::mat4 lightView = glm::lookAt(position, position + sun.direction, glm::vec3{0.f, 1.f, 0.f});
    glm::mat4 lightProjection = glm::ortho(-shadowMapSize, shadowMapSize,
                                           -shadowMapSize, shadowMapSize,
                                           0.1f, 200.f);

    sun.lightSpaceMatrix = lightProjection * lightView;
}

void DirectionalLight::UpdateSun() {
    glNamedBufferSubData(uboLightData, 0, sizeof(DirectionalLightData), &sun);
}

const DirectionalLightData& DirectionalLight::GetSun() const {
    return sun;
}

void DirectionalLight::SetSun(const DirectionalLightData& sun) {
    DirectionalLight::sun = sun;
    UpdateSun();
}

glm::vec3 DirectionalLight::DirectionVector(float pitch, float yaw) {
    glm::vec3 Result;
    Result.x = std::cos(pitch) * std::cos(yaw);
    Result.y = std::sin(pitch);
    Result.z = std::cos(pitch) * std::sin(-yaw);
    return Result;
}

void DirectionalLight::InitializeFrameBuffer() {
    glCreateFramebuffers(1, &shadowMapFrameBuffer);

    glCreateTextures(GL_TEXTURE_2D, 1, &shadowMapTexture);
    glTextureParameteri(shadowMapTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(shadowMapTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(shadowMapTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(shadowMapTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureStorage2D(shadowMapTexture, 1, GL_DEPTH_COMPONENT24, shadowMapResolution, shadowMapResolution);

    glm::vec4 borderColor{1.f};
    glTextureParameterfv(shadowMapTexture, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));
    glNamedFramebufferTexture(shadowMapFrameBuffer, GL_DEPTH_ATTACHMENT, shadowMapTexture, 0);

    MLG_ASSERT_MSG(glCheckNamedFramebufferStatus(shadowMapFrameBuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                   "Shadowmap Framebuffer is not complete");

    shadowMapMaterial = AssetManager::GetAsset<MaterialAsset>("res/config/shadowmap_material.json");
}

void DirectionalLight::BindShadowMap() {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);
}

DirectionalLight::~DirectionalLight() {
    glDeleteBuffers(1, &uboLightData);
    glDeleteFramebuffers(1, &shadowMapFrameBuffer);
    glDeleteTextures(1, &shadowMapTexture);
}

DirectionalLight* DirectionalLight::instance;

DirectionalLight* DirectionalLight::GetInstance() {
    if (instance == nullptr)
        instance = new DirectionalLight();

    return instance;
}

void DirectionalLight::BindShadowMapShader() {
    shadowMapMaterial->GetShaderProgram()->Activate();
}

std::weak_ptr<class ShaderProgram> DirectionalLight::GetShadowShaderProgram() {
    return shadowMapMaterial->GetShaderProgram();
}

uint32_t DirectionalLight::GetShadowMap() {
    return shadowMapTexture;
}

