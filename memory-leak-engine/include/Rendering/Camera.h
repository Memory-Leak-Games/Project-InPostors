#pragma once

#include <Core/Time.h>
#include "glad/glad.h"

namespace mlg {

    class Camera {
    private:
        static Camera* instance;

        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;

        float pitch = 75.0f;
        float yaw = -180.0f;
        float roll;

        GLuint uboTransformMatrices;

        glm::vec<2, int> resolution{};
        float fow = 90.f;

        Camera();

    public:
        static Camera* GetInstance();
        ~Camera();


        void SetPosition(glm::vec3 newPosition);

        void SetRotation(float pitch, float yaw);
        void SetRotation(glm::vec3 frontVector, glm::vec3 upVector);

        void LookAt(glm::vec3 lookAtPosition);
        void Orbit(glm::vec3 origin, float pitch, float yaw, float radius);

        void SetResolution(const glm::vec<2, int> &newResolution);
        void SetFow(float newFow);

        [[nodiscard]] glm::mat4 GetCameraProjectionMatrix(int resolutionX, int resolutionY) const;

        [[nodiscard]] const glm::vec3 &GetPosition() const;
        [[nodiscard]] const glm::vec3 &GetFront() const;
        [[nodiscard]] const glm::vec3 &GetUp() const;
        [[nodiscard]] glm::vec3 GetRight() const;

    private:
        void UpdateProjection();
        void UpdateView();
    };
}
