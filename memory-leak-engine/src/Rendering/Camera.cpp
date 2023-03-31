#include "Rendering/Camera.h"
#include "Rendering/CommonUniformBuffer.h"

#include "Macros.h"

using namespace mlg;

Camera* Camera::instance;

Camera::Camera() : front(0.f, 0.f, 1.f), up(0.f, 1.f, 0.f), position(0.f), uboTransformMatrices(0),
                   resolution({1280, 720})
{
    SPDLOG_INFO("Initializing Camera");

    SetRotation(pitch, yaw);
    UpdateView();
    UpdateProjection();
}

Camera::~Camera()
{
    SPDLOG_INFO("Stopping Camera");
}

glm::mat4 Camera::GetCameraProjectionMatrix(int resolutionX, int resolutionY) const
{
    return glm::perspective(glm::radians(fow), static_cast<float>(resolutionX) / static_cast<float>(resolutionY), 0.1f, 100.f);
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
    CommonUniformBuffer::instance->uniforms.projection = GetCameraProjectionMatrix(resolution.x, resolution.y);
}

void Camera::UpdateView()
{
    CommonUniformBuffer::instance->uniforms.view = glm::lookAt(position, position + front, up);
}

void Camera::SetPosition(glm::vec3 newPosition)
{
    position = newPosition;
    UpdateView();
}

void Camera::SetRotation(float pitch, float yaw)
{
    front.x = glm::cos(pitch) * glm::cos(yaw - glm::half_pi<float>());
    front.y = glm::sin(pitch);
    front.z = glm::cos(pitch) * glm::sin(yaw - glm::half_pi<float>());
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
    return glm::cross(front, up);
}

Camera* Camera::GetInstance() {
    if (!instance)
        instance = new Camera;

    return instance;
}

glm::mat4 Camera::GetCameraViewMatrix() const {
    return CommonUniformBuffer::instance->uniforms.view;
}
