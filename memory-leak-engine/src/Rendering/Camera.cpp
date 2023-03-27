#include "Rendering/Camera.h"
#include "Core/HID/Input.h"
#include "glm/gtc/type_ptr.hpp"

#include "Macros.h"

using namespace mlg;

Camera::Camera() : front(0.f, 0.f, 1.f), up(0.f, 1.f, 0.f), position(0.f), uboTransformMatrices(0),
                   resolution({1280, 720})
{
    SPDLOG_INFO("Initializing Camera");
    glGenBuffers(1, &uboTransformMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboTransformMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboTransformMatrices);
    SetRotation(pitch, yaw);
    UpdateView();
    UpdateProjection();
}

Camera::~Camera()
{
    SPDLOG_INFO("Stopping Camera");
    // TODO: segmentation fault ??? <-- needs to repair
//    glDeleteBuffers(1, &uboTransformMatrices);
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

std::shared_ptr<Camera> Camera::instance;

std::shared_ptr<Camera> Camera::GetInstance() {
    if (!instance)
        instance = std::shared_ptr<Camera>(new Camera);

    return instance;
}

// TODO: Transfer this to camera component
/**
 * Processes movement of this camera
 * @param movement CameraMovement enum value corresponding to movement direction
 * @param deltaTime Time elapsed since last frame
 */
void Camera::ProcessMovement(CameraMovement movement, float deltaTime) {
    float velocity = speed * deltaTime;
    switch(movement)
    {
        case FORWARD:
        {
            position += front * velocity;
            break;
        }
        case BACKWARD:
        {
            position -= front * velocity;
            break;
        }
        case LEFT:
        {
            //glm::cross(front, up) = right vector
            position -= glm::cross(front, up) * velocity;
            break;
        }
        case RIGHT:
        {
            position +=  glm::cross(front, up) * velocity;
            break;
        }
        case UP:
        {
            position += up * velocity;
            height = position.y;
            break;
        }
        case DOWN:
        {
            position -= up * velocity;
            height = position.y;
            break;
        }
        default: break;
    }
    // Height correction (#itjustworks)
    if (isFixedHeight)
        position.y = height;

    UpdateView();
}

// TODO: Transfer this to camera component
/**
 * Processes rotation of this camera
 * @param xOffset Pitch offset
 * @param yOffset Yaw offset
 * @param pitchConstrain Should pitch be capped between (-89; 89)
 */
void Camera::ProcessRotation(float xOffset, float yOffset, bool pitchConstrain) {
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    pitch += xOffset;
    yaw += yOffset;

    if (pitchConstrain) {
        // I love oneliners
        pitch = pitch > 89.0f ? 89.0f : pitch;
        pitch = pitch < -89.0f ? -89.0f : pitch;
    }
    SPDLOG_DEBUG(pitch);

    SetRotation(pitch, yaw); //no need to call UpdateView as it is called by SetRotation()
}


// TODO: Transfer this to camera component
void Camera::ProcessZoom(float offset) {
    zoom -= static_cast<float>(offset);
    zoom = zoom < 1.0f ? 1.0f : zoom;
    zoom = zoom > MAX_ZOOM ? MAX_ZOOM : zoom;
}
