#pragma once

#include "Gameplay/SceneComponent.h"

namespace mlg {

    class CameraComponent : public SceneComponent {
    private:

        struct Projection {
            float near;
            float far;

            Projection(float nearPlane, float farPlane);
            virtual ~Projection() = default;

            virtual glm::mat4 CalculateProjection(float aspectRatio) = 0;
        };

        struct OrthoProjection : Projection {
            float size;

            OrthoProjection(float size, float nearPlane, float farPlane);
            glm::mat4 CalculateProjection(float aspectRatio) override;
        };

        struct PerspectiveProjection : Projection {
            float fov;

            PerspectiveProjection(float fov, float nearPlane, float farPlane);
            glm::mat4 CalculateProjection(float aspectRatio) override;
        };

        std::unique_ptr<Projection> projection;

    public:
        CameraComponent() = delete;

        CameraComponent(const std::weak_ptr<Entity>& owner, const std::string& name);

        void SetOrtho(float size, float nearPlane, float farPlane);
        void SetPerspective(float fov, float nearPlane, float farPlane);

        void Update() override;
        void OnWindowResize(const class Event& event);

#ifdef DEBUG
        void UpdateImGUI();
#endif
    };

} // mlg
