#include "LowLevelRenderer/Camera.h"
#include "glm/gtc/type_ptr.hpp"

#include "Macros.h"

using namespace mlg;

Camera::Camera() : front(0.f, 0.f, 1.f), up(0.f, 1.f, 0.f), position(0.f), uboTransformMatrices(0),
                   resolution({1280, 720})
{
    glGenBuffers(1, &uboTransformMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboTransformMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboTransformMatrices);
    UpdateView();
}

Camera::~Camera()
{
    glDeleteBuffers(1, &uboTransformMatrices);
}

glm::mat4 Camera::GetCameraProjectionMatrix(int resolutionX, int resolutionY) const
{
    return glm::perspective(glm::radians(fow), static_cast<float>(resolutionX) / static_cast<float>(resolutionY), 0.1f, 1000.f);
}

void Camera::SetResolution(const glm::vec<2, int> &newResolution)
{
    if (newResolution != resolution)
    {
        resolution = newResolution;
        UpdateProjection();
    }
}

void Camera::SetFow(float newFow)
{
    if (newFow != fow)
    {
        fow = newFow;
        UpdateProjection();
    }
}

void Camera::UpdateProjection()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboTransformMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                    glm::value_ptr(GetCameraProjectionMatrix(resolution.x, resolution.y)));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::UpdateView()
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboTransformMatrices);
    glm::mat4 ViewMatrix = glm::lookAt(position, position + front, up);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(ViewMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(position));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::SetPosition(glm::vec3 newPosition)
{
    position = newPosition;
    UpdateView();
}

void Camera::SetRotation(float x, float y)
{
    front.x = glm::cos(x) * glm::cos(y - glm::half_pi<float>());
    front.y = glm::sin(x);
    front.z = glm::cos(x) * glm::sin(y - glm::half_pi<float>());
    front = glm::normalize(front);

    glm::vec3 Right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), front));
    up = glm::normalize(glm::cross(front, Right));

    UpdateView();
}

void Camera::SetRotation(glm::vec3 frontVector, glm::vec3 upVector) {
    front = frontVector;
    up = upVector;

    UpdateView();
}

void Camera::LookAt(glm::vec3 lookAtPosition)
{
    front = glm::normalize(lookAtPosition - position);
    glm::vec3 Right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), front));
    up = glm::normalize(glm::cross(front, Right));

    UpdateView();
}

void Camera::Orbit(glm::vec3 origin, float pitch, float yaw, float radius)
{
    glm::vec3 NewPosition(0.f);
    NewPosition.x = origin.x + radius * glm::sin(pitch) * glm::cos(yaw);
    NewPosition.y = origin.y + radius * glm::cos(pitch);
    NewPosition.z = origin.z + radius * glm::sin(pitch) * glm::sin(yaw);

    SetPosition(NewPosition);
}

const glm::vec3 &Camera::GetPosition() const
{
    return position;
}

const glm::vec3 &Camera::GetFront() const
{
    return front;
}

const glm::vec3 &Camera::GetUp() const
{
    return up;
}

glm::vec3 Camera::GetRight() const
{
    glm::vec3 Result = glm::cross(front, up);
    return Result;
}

void Camera::Update(float deltaSeconds)
{
}

std::shared_ptr<Camera> Camera::instance;

std::shared_ptr<Camera> Camera::GetInstance() {
    if (!instance)
        instance = std::shared_ptr<Camera>(new Camera);

    return instance;
}
