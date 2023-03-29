#pragma once

#include "glad/glad.h"
#include <SceneGraph/Transform.h>

namespace mlg {

    class Gizmos {
    private:
        static uint32_t lineVBO;
        static uint32_t lineVAO;
        static uint32_t boxVBO;
        static uint32_t boxVAO;
        static uint32_t boxEBO;
        static uint32_t sphereVBO;
        static uint32_t sphereVAO;
        static uint32_t sphereEBO;
        static uint32_t sphereIndicesCount;
        static uint32_t pointVBO;
        static uint32_t pointVAO;

        static class ShaderProgram* shader;
        static glm::vec4 defaultColor;

        static void GenerateSphere(std::vector<GLfloat>& vertices, std::vector<uint32_t>& indices, int LOD);

    public:
        static void Initialize();
        static void Stop();

        static void DrawGizmos();

        static void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color = defaultColor, bool alwaysFront = false);
        static void DrawBox(glm::vec3 position, glm::vec3 size = {1, 1, 1}, glm::quat rotation = glm::quat(), glm::vec4 color = defaultColor, bool alwaysFront = false);
        static void DrawBox(Transform& transform, glm::vec4 color = defaultColor, bool alwaysFront = false);
        static void DrawSphere(glm::vec3 position, float radius = 1, glm::vec4 color = defaultColor, bool alwaysFront = false);
        static void DrawPoint(glm::vec3 position, glm::vec4 color = defaultColor, bool alwaysFront = false);
        static void DrawSizedPoint(glm::vec3 position, float size = 2, glm::vec4 color = defaultColor, bool alwaysFront = false);
    };

}// namespace mlg