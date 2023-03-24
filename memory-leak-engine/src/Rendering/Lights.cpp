#include "Rendering/Lights.h"
#include "Rendering/Gizmos/SphereGizmo.h"

using namespace mlg;

Lights::Lights()
{
    InitializeLights();

    glGenBuffers(1, &uboLightData);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLightData);
    glBufferData(GL_UNIFORM_BUFFER, 208, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLightData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    UpdateSun();
    UpdateBulb();
    UpdateSpotLights();
}

void Lights::InitializeLights()
{
    sun.color = glm::vec4(0.f);
    sun.direction = glm::normalize(glm::vec3(-0.5f, -0.5f, -0.5f));

    bulb.color = glm::vec4(1.f);
    bulb.position = glm::vec3(-2.f, 2.f, -5.f);
    bulb.linear = 0.07f;
    bulb.quadratic = 0.017f;

    spotLights[0].color = glm::vec4(0.f);
    spotLights[0].position = glm::vec3(-5.f, 4.5f, 18.8f);
    spotLights[0].direction = glm::normalize(glm::vec3(-0.5f, -0.5f, 0.5f));
    spotLights[0].linear = 0;
    spotLights[0].quadratic = 0;
    spotLights[0].cutOff = glm::radians(12.5f);
    spotLights[0].outerCutOff = glm::radians(17.5f);

    spotLights[1].color = glm::vec4(0.f);
    spotLights[1].position = glm::vec3(-3.3f, 4.6f, 10.5f);
    spotLights[1].direction = glm::normalize(glm::vec3(0.5f, -0.5f, 0.5f));
    spotLights[1].linear = 0;
    spotLights[1].quadratic = 0.017f;
    spotLights[1].cutOff = glm::radians(12.5f);
    spotLights[1].outerCutOff = glm::radians(17.5f);
}

void Lights::UpdateSun()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboLightData);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 32, &sun);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Lights::UpdateBulb()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboLightData);
    glBufferSubData(GL_UNIFORM_BUFFER, 32, 48, &bulb);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Lights::UpdateSpotLights()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboLightData);
    for (size_t i = 0; i < 2; i++)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, 80 + i * 64, 64, &spotLights[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

const DirectionalLight &Lights::GetSun() const
{
    return sun;
}

const PointLight &Lights::GetBulb() const
{
    return bulb;
}

const SpotLight &Lights::GetSpotLightOne() const
{
    return spotLights[0];
}

const SpotLight &Lights::GetSpotLightTwo() const
{
    return spotLights[1];
}

void Lights::SetSun(const DirectionalLight &sun)
{
    Lights::sun = sun;
    UpdateSun();
}

void Lights::SetBulb(const PointLight &bulb)
{
    Lights::bulb = bulb;
    UpdateBulb();
}

void Lights::SetSpotLightOne(const SpotLight &spotLightOne)
{
    spotLights[0] = spotLightOne;
    UpdateSpotLights();
}

void Lights::SetSpotLightTwo(const SpotLight &spotLightTwo)
{
    spotLights[1] = spotLightTwo;
    UpdateSpotLights();
}

Lights::~Lights()
{
    glDeleteBuffers(1, &uboLightData);
}

void Lights::DrawGizmos()
{
    //SphereGizmo::Draw(bulb.position, 1.0f, 24, bulb.color);
}

glm::vec3 Lights::DirectionVector(float pitch, float yaw) {
    glm::vec3 Result;
    Result.x = std::cos(pitch) * std::cos(yaw);
    Result.y = std::sin(pitch);
    Result.z = std::cos(pitch) * std::sin(-yaw);
    return Result;
}
