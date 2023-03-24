#pragma once

#include "glad/glad.h"
#include "Rendering/ShaderWrapper.h"

namespace mlg {
    class OldGizmo
    {
    protected:
        static GLuint VAO;
        static GLuint VBO;
        static GLuint EBO;
        static std::shared_ptr<ShaderWrapper> Shader;

    public:
        static void Initialize();
    };
}