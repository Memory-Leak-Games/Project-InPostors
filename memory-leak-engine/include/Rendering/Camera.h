#pragma once

namespace mlg {

    class Camera {
    public:
        struct Projection {
            float nearPlane;
            float farPlane;

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

    private:
        std::unique_ptr<Projection> projection;
        std::shared_ptr<class Transform> transform;

        bool isProjectionDirty;
        bool isViewDirty;

        bool wasProjectionDirty;
        bool wasViewDirty;

    public:
        Camera();

        void Update();

        void OnWindowResize(const class Event& event);

        void SetOrtho(float size, float nearPlane, float farPlane);
        void SetPerspective(float fov, float nearPlane, float farPlane);

        void SetActive();
        bool IsActive();

        [[nodiscard]] bool GetWasProjectionDirty() const;
        [[nodiscard]] bool GetWasViewDirty() const;

        Transform& GetTransform();
        Projection* GetProjection();

        friend class CameraComponent;
    private:
        void CalculateProjection();
        void CalculateView();
    };

}// namespace mlg
