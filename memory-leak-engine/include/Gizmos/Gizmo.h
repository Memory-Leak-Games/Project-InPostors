#pragma once

#include <memory>
#include "glad/glad.h"
#include "ShaderWrapper.h"

class Gizmo
{
protected:
    static GLuint VAO;
    static GLuint VBO;
    static GLuint EBO;
    static std::shared_ptr<ShaderWrapper> Shader;

public:
    static void Initialize();
};
