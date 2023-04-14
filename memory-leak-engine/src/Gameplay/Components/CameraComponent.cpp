#include "Gameplay/Components/CameraComponent.h"

#include "Core/Window.h"
#include "Rendering/CommonUniformBuffer.h"
#include "Events/WindowEvent.h"

namespace mlg {

    CameraComponent::Projection::Projection(float near, float far)
            : near(near), far(far) {}

    CameraComponent::OrthoProjection::OrthoProjection(float size, float near, float far)
            : size(size), Projection(near, far) {}

    glm::mat4 CameraComponent::OrthoProjection::CalculateProjection(float aspectRatio) {
        float halfHeight = size * 0.5f;
        float halfWidth = (size * aspectRatio) * 0.5f;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
    }

    CameraComponent::PerspectiveProjection::PerspectiveProjection(float fov, float near, float far)
            : fov(fov), Projection(near, far) {}

    glm::mat4 CameraComponent::PerspectiveProjection::CalculateProjection(float aspectRatio) {
        return glm::perspective(fov, aspectRatio, near, far);
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

    void CameraComponent::SetOrtho(float size, float near, float far) {
        float aspectRatio = Window::GetInstance()->GetAspectRatio();
        projection = std::make_unique<OrthoProjection>(size, near, far);

        CommonUniformBuffer::SetProjection(projection->CalculateProjection(aspectRatio));
    }

    void CameraComponent::SetPerspective(float fov, float near, float far) {
        float aspectRatio = Window::GetInstance()->GetAspectRatio();
        projection = std::make_unique<PerspectiveProjection>(fov, near, far);

        CommonUniformBuffer::SetProjection(projection->CalculateProjection(aspectRatio));
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