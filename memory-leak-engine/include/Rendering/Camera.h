#pragma once

#include "glad/glad.h"

namespace mlg {
    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    class Camera {
    private:
        static std::shared_ptr<Camera> instance;

        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;

        GLuint uboTransformMatrices;

        glm::vec<2, int> resolution{};
        float fow = 90.f;

        Camera();

    public:
        static std::shared_ptr<Camera> GetInstance();
        ~Camera();


        void SetPosition(glm::vec3 newPosition);

        void SetRotation(float x, float y);
        void SetRotation(glm::vec3 frontVector, glm::vec3 upVector);

        void LookAt(glm::vec3 lookAtPosition);
        void Orbit(glm::vec3 origin, float pitch, float yaw, float radius);

        void SetResolution(const glm::vec<2, int> &newResolution);
        void SetFow(float newFow);

        [[nodiscard]] glm::mat4 GetCameraProjectionMatrix(int resolutionX, int resolutionY) const;

        [[nodiscard]] const glm::vec3 &GetPosition() const;
        [[nodiscard]] const glm::vec3 &GetFront() const;
        [[nodiscard]] const glm::vec3 &GetUp() const;
        [[nodiscard]] inline glm::vec3 GetRight() const;

        void ProcessMovement(CameraMovement movement, float deltaTime);
        void ProcessRotation(const float xOffset, const float yOffset, bool pitchConstrain = true);
        void ProcessZoom(float offset);

    private:
        void UpdateProjection();
        void UpdateView();
    };
}
