#pragma once

#include "glad/glad.h"
#include <SceneGraph/Transform.h>

namespace mlg {

    class Gizmos {
    private:

        struct GizmoObject {
            uint32_t VBO;
            uint32_t VAO;
            uint32_t EBO = 0;
            int32_t elements;
            uint32_t primitive = GL_LINES;

            glm::mat4 world;
            glm::vec4 color;
            bool alwaysFront; //TODO: Make this work if false
            float pointSize = 1.0;

            //float ttl = 0; // Time to live
        };

        static uint32_t lineVBO;
        static uint32_t lineVAO;
        static uint32_t boxVBO;
        static uint32_t boxVAO;
        static uint32_t boxEBO;
        static uint32_t sphereVBO;
        static uint32_t sphereVAO;
        static uint32_t sphereEBO;
        static int32_t sphereIndicesCount;
        static uint32_t pointVBO;
        static uint32_t pointVAO;

        static class ShaderProgram* shader;
        static glm::vec4 defaultColor;

        static std::vector<GizmoObject> gizmoInstances;

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
        static void DrawSizedPoint(glm::vec3 position, float pointSize = 2, glm::vec4 color = defaultColor, bool alwaysFront = false);
    };

}// namespace mlg