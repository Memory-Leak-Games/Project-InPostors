#include "Gameplay/Components/CameraComponent.h"

#include "Core/Window.h"
#include "Rendering/CommonUniformBuffer.h"

namespace mlg {
    void CameraComponent::SetOrtho(float size, float near, float far) {
        float aspectRatio = Window::GetInstance()->GetAspectRatio();

        float halfHeight = size * 0.5f;
        float halfWidth = (size * aspectRatio) * 0.5f;
        glm::mat4 projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
        CommonUniformBuffer::SetProjection(projectionMatrix);
    }

    void CameraComponent::SetPerspective(float fov, float near, float far) {
        float aspectRatio = Window::GetInstance()->GetAspectRatio();

        glm::mat4 projectionMatrix = glm::perspective(fov, aspectRatio, near, far);
        CommonUniformBuffer::SetProjection(projectionMatrix);
    }

    void CameraComponent::Update() {
        const glm::vec3 position = GetTransform().GetPosition();
        const glm::vec3 forward = GetTransform().GetForwardVector();
        const glm::vec3 up = GetTransform().GetUpVector();

        glm::mat4 viewMatrix = glm::lookAt(position, position + forward, up);

        CommonUniformBuffer::SetView(viewMatrix);

#ifdef DEBUG
        UpdateImGUI();
#endif
    }

    CameraComponent::CameraComponent(const std::weak_ptr<Entity>& owner, const std::string& name)
    : SceneComponent(owner, name) {}

#ifdef DEBUG
    void CameraComponent::UpdateImGUI() {
        ImGui::Begin("Camera");

        glm::vec3 position = GetTransform().GetWorldPosition();
        glm::vec3 rotation = glm::degrees(GetTransform().GetEulerRotation());

        ImGui::DragFloat3("Camera Position", (float*) &position, 0.1f);
        ImGui::DragFloat3("Camera Rotation", (float*) &rotation, 1.f);

        GetTransform().SetPosition(position);
        GetTransform().SetRotation({glm::radians(rotation)});

        ImGui::End();
    }
#endif
} // mlg