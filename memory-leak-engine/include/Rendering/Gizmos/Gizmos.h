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
        static std::shared_ptr<ShaderWrapper> Shader;

    public:
        static void Initialize();
        static void Stop();

        static void DrawLine(glm::vec3 Start, glm::vec3 End, glm::vec4 Color={0, 1, 0, 1});
        static void DrawBox(glm::vec3 Position, glm::vec3 Size, glm::quat Rotation, glm::vec4 Color={0, 1, 0, 1});
        static void DrawBox(Transform& transform, glm::vec4 Color={0, 1, 0, 1});
    };

}