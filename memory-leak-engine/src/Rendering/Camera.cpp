#include "Rendering/Camera.h"

#include "Core/Window.h"
#include "Events/WindowEvent.h"

#include "Rendering/CommonUniformBuffer.h"
#include "Rendering/Renderer.h"
#include "SceneGraph/Transform.h"

namespace mlg {
    Camera::Projection::Projection(float nearPlane, float farPlane)
        : nearPlane(nearPlane), farPlane(farPlane) {}

    Camera::OrthoProjection::OrthoProjection(float size, float nearPlane, float farPlane)
        : size(size), Projection(nearPlane, farPlane) {}

    glm::mat4 Camera::OrthoProjection::CalculateProjection(float aspectRatio) {
        float halfHeight = size * 0.5f;
        float halfWidth = (size * aspectRatio) * 0.5f;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
    }

    Camera::PerspectiveProjection::PerspectiveProjection(float fov, float nearPlane, float farPlane)
        : fov(fov), Projection(nearPlane, farPlane) {}

    glm::mat4 Camera::PerspectiveProjection::CalculateProjection(float aspectRatio) {
        return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
    }

    void Camera::SetOrtho(float size, float nearPlane, float farPlane) {
        isProjectionDirty = true;
        projection = std::make_unique<OrthoProjection>(size, nearPlane, farPlane);
    }

    void Camera::SetPerspective(float fov, float nearPlane, float farPlane) {
        isProjectionDirty = true;
        projection = std::make_unique<PerspectiveProjection>(fov, nearPlane, farPlane);
    }

    Camera::Camera()
        : isProjectionDirty(true), isViewDirty(true), wasViewDirty(true), wasProjectionDirty(true), transform(std::make_shared<Transform>()) {
        Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize, [this](const Event& event) {
            this->OnWindowResize(event);
        });

        transform->onTransformationChange.append([this]() {
            this->isViewDirty = true;
        });
    }

    void Camera::SetActive() {
        Renderer::GetInstance()->SetCurrentCamera(this);
    }

    bool Camera::IsActive() {
        return Renderer::GetInstance()->GetCurrentCamera() == this;
    }

    void Camera::OnWindowResize(const Event& event) {
        auto& windowResizeEvent = (WindowResizeEvent&) event;
        CommonUniformBuffer::SetProjection(projection->CalculateProjection(windowResizeEvent.GetAspectRatio()));
        isProjectionDirty = true;
    }

    Transform& Camera::GetTransform() {
        return *transform;
    }

    bool Camera::GetWasProjectionDirty() const {
        return wasProjectionDirty;
    }

    bool Camera::GetWasViewDirty() const {
        return wasViewDirty;
    }

    Camera::Projection* Camera::GetProjection() {
        return projection.get();
    }

    void Camera::Update() {
        if (!IsActive())
            return;

        CalculateView();
        CalculateProjection();
    }

    void Camera::CalculateProjection() {
        wasProjectionDirty = false;

        if (!isProjectionDirty)
            return;

        float aspectRatio = Window::GetInstance()->GetAspectRatio();
        CommonUniformBuffer::SetProjection(projection->CalculateProjection(aspectRatio));

        isProjectionDirty = false;
        wasProjectionDirty = true;
    }

    void Camera::CalculateView() {
        wasViewDirty = false;

        if (!isViewDirty)
            return;

        const glm::vec3 position = GetTransform().GetPosition();
        const glm::vec3 forward = GetTransform().GetForwardVector();
        const glm::vec3 up = GetTransform().GetUpVector();

        const glm::mat4 viewMatrix = glm::lookAt(position, position + forward, up);

        CommonUniformBuffer::SetView(viewMatrix);

        isViewDirty = false;
        wasViewDirty = true;
    }
}// namespace mlg