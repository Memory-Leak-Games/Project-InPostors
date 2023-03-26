#include "Rendering/Lights.h"
#include "Rendering/Gizmos/SphereGizmo.h"

using namespace mlg;

Lights::Lights()
{
    InitializeLights();

    glGenBuffers(1, &uboLightData);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLightData);
    glBufferData(GL_UNIFORM_BUFFER, 32, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLightData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    UpdateSun();
}

void Lights::InitializeLights()
{
    sun.color = glm::vec4(1.f);
    sun.direction = glm::normalize(glm::vec3(-0.5f, -0.5f, 0.5f));
}

void Lights::UpdateSun()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboLightData);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 32, &sun);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

const DirectionalLight &Lights::GetSun() const
{
    return sun;
}

void Lights::SetSun(const DirectionalLight &sun)
{
    Lights::sun = sun;
    UpdateSun();
}

Lights::~Lights()
{
    glDeleteBuffers(1, &uboLightData);
}

glm::vec3 Lights::DirectionVector(float pitch, float yaw) {
    glm::vec3 Result;
    Result.x = std::cos(pitch) * std::cos(yaw);
    Result.y = std::sin(pitch);
    Result.z = std::cos(pitch) * std::sin(-yaw);
    return Result;
}
