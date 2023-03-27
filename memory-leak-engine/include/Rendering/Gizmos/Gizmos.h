#pragma once

#include "Rendering/ShaderWrapper.h"
#include "glad/glad.h"
#include <SceneGraph/Transform.h>

namespace mlg {

    class Gizmos {
    private:
        static GLuint LineVBO;
        static GLuint LineVAO;
        static GLuint BoxVBO;
        static GLuint BoxVAO;
        static GLuint BoxEBO;
        static GLuint SphereVBO;
        static GLuint SphereVAO;
        static GLuint SphereEBO;

        static std::shared_ptr<ShaderWrapper> Shader;
        static glm::vec4 DefaultColor;

        static void GenerateSphere(std::vector<GLfloat>& Vertices, std::vector<GLuint>& Indices, int LOD);

    public:
        static void Initialize();
        static void Stop();

        static void DrawLine(glm::vec3 Start, glm::vec3 End, glm::vec4 Color=DefaultColor, bool AlwaysFront=false);
        static void DrawBox(glm::vec3 Position, glm::vec3 Size={1, 1, 1}, glm::quat Rotation=glm::quat(), glm::vec4 Color=DefaultColor, bool AlwaysFront=false);
        static void DrawBox(Transform& Transform, glm::vec4 Color=DefaultColor, bool AlwaysFront=false);
        static void DrawSphere(glm::vec3 Position, float Radius=1, glm::vec4 Color=DefaultColor, bool AlwaysFront=false);
    };

}