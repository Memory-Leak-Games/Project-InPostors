#pragma once

#include "glad/glad.h"
#include "Rendering/ShaderProgram.h"

namespace mlg {
    class Gizmo
    {
    protected:
        static GLuint VAO;
        static GLuint VBO;
        static GLuint EBO;
        static std::shared_ptr<ShaderProgram> Shader;

    public:
        static void Initialize();
    };
}