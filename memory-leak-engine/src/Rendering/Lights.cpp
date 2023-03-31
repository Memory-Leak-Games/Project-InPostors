#include "Rendering/DirectionalLight.h"

using namespace mlg;

DirectionalLight::DirectionalLight() {
    InitializeLights();

    glCreateBuffers(1, &uboLightData);
    glNamedBufferData(uboLightData, sizeof(DirectionalLightData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLightData);

    UpdateSun();
}

void DirectionalLight::InitializeLights() {
    sun.direction = glm::normalize(glm::vec3(0.5f, -0.5f, -0.5f));
    sun.diffuse = glm::vec3(0.7f);
    sun.ambient = glm::vec3(0.3f);
    sun.specular = glm::vec3(0.1f);

    glm::vec3 position = glm::vec3(-5.f);

    glm::mat4 lightView = glm::lookAt(position, position + sun.direction, glm::vec3{0.f, 1.f, 0.f});
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.f);

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

DirectionalLight::~DirectionalLight() {
    glDeleteBuffers(1, &uboLightData);
}

glm::vec3 DirectionalLight::DirectionVector(float pitch, float yaw) {
    glm::vec3 Result;
    Result.x = std::cos(pitch) * std::cos(yaw);
    Result.y = std::sin(pitch);
    Result.z = std::cos(pitch) * std::sin(-yaw);
    return Result;
}
