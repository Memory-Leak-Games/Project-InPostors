#include "Gameplay/Components/CameraComponent.h"

#include "Core/Window.h"
#include "Rendering/CommonUniformBuffer.h"
#include "Events/WindowEvent.h"

namespace mlg {

    CameraComponent::Projection::Projection(float nearPlane, float farPlane)
            : nearPlane(nearPlane), farPlane(farPlane) {}

    CameraComponent::OrthoProjection::OrthoProjection(float size, float nearPlane, float farPlane)
            : size(size), Projection(nearPlane, farPlane) {}

    glm::mat4 CameraComponent::OrthoProjection::CalculateProjection(float aspectRatio) {
        float halfHeight = size * 0.5f;
        float halfWidth = (size * aspectRatio) * 0.5f;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
    }

    CameraComponent::PerspectiveProjection::PerspectiveProjection(float fov, float nearPlane, float farPlane)
            : fov(fov), Projection(nearPlane, farPlane) {}

    glm::mat4 CameraComponent::PerspectiveProjection::CalculateProjection(float aspectRatio) {
        return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
    }

    CameraComponent::CameraComponent(const std::weak_ptr<Entity>& owner, const std::string& name)
            : SceneComponent(owner, name) {
        Window::GetInstance()->GetEventDispatcher()->appendListener(EventType::WindowResize,
                                                                    [this](const Event& event) {
            this->OnWindowResize(event);
        });
    }

    void CameraComponent::OnWindowResize(const Event& event) {
        auto& windowResizeEvent = (WindowResizeEvent&) event;
        CommonUniformBuffer::SetProjection(projection->CalculateProjection(windowResizeEvent.GetAspectRatio()));
    }

    void CameraComponent::SetOrtho(float size, float nearPlane, float farPlane) {
        projection = std::make_unique<OrthoProjection>(size, nearPlane, farPlane);
    }

    void CameraComponent::SetPerspective(float fov, float nearPlane, float farPlane) {
        projection = std::make_unique<PerspectiveProjection>(fov, nearPlane, farPlane);
    }

    void CameraComponent::Update() {
        const glm::vec3 position = GetTransform().GetPosition();
        const glm::vec3 forward = GetTransform().GetForwardVector();
        const glm::vec3 up = GetTransform().GetUpVector();

        glm::mat4 viewMatrix = glm::lookAt(position, position + forward, up);

        float aspectRatio = Window::GetInstance()->GetAspectRatio();

        CommonUniformBuffer::SetView(viewMatrix);
        CommonUniformBuffer::SetProjection(projection->CalculateProjection(aspectRatio));

#ifdef DEBUG
        UpdateImGUI();
#endif
    }

#ifdef DEBUG

    void CameraComponent::UpdateImGUI() {
        ImGui::Begin("Camera");

        glm::vec3 position = GetTransform().GetWorldPosition();
        glm::vec3 rotation = glm::degrees(GetTransform().GetEulerRotation());

        ImGui::DragFloat3("Camera Position", (float*) &position, 0.1f);
        ImGui::DragFloat3("Camera Rotation", (float*) &rotation, 1.f);

        GetTransform().SetPosition(position);
        GetTransform().SetRotation({glm::radians(rotation)});

        auto* orthoProjection = dynamic_cast<OrthoProjection*>(projection.get());
        auto* perspectiveProjection = dynamic_cast<PerspectiveProjection*>(projection.get());

        if (orthoProjection)
        {
            float size = orthoProjection->size;
            ImGui::DragFloat("Ortho Size", &size, 0.1);
            orthoProjection->size = size;
        }
        else if (perspectiveProjection) {
            float fow = perspectiveProjection->fov;
            ImGui::DragFloat("Fow", &fow, 0.1);
            perspectiveProjection->fov = fow;
        }

        ImGui::End();
    }

#endif

} // mlg