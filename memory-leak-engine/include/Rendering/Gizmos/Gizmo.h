#pragma once

#include "glad/glad.h"
#include "Rendering/ShaderWrapper.h"

namespace mlg {

    class Gizmo {
    private:
        static GLuint LineVBO;
        static GLuint LineVAO;
        static std::shared_ptr<ShaderWrapper> Shader;

    public:
        static void Initialize();
        static void Stop();

        static void Line(glm::vec3 Start, glm::vec3 End, glm::vec4 Color);
    };

}