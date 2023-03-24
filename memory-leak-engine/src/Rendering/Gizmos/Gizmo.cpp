#include "Rendering/Gizmos/Gizmo.h"

using namespace mlg;

GLuint Gizmo::LineVAO;
GLuint Gizmo::LineVBO;
std::shared_ptr<ShaderWrapper> Gizmo::Shader;

GLfloat LineVertices[] = {
    0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f
};

void mlg::Gizmo::Initialize()
{
    // Load gizmo shader
    Shader = std::make_shared<ShaderWrapper>("res/shaders/gizmo.vert", "res/shaders/gizmo.frag");

    // Generate line gizmo
    glGenBuffers(1, &LineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), LineVertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &LineVAO);
    glBindVertexArray(LineVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
    glEnableVertexAttribArray(0);
}

void mlg::Gizmo::Stop()
{

}

void mlg::Gizmo::Line(glm::vec3 Start, glm::vec3 End, glm::vec4 Color)
{
    glm::mat4 World = glm::mat4(1.0f);
    World = glm::translate(World, Start);
    // Use position diff to scale line (which is from [0, 0, 0] to [1, 1, 1])
    World = glm::scale(World, End - Start);

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);

    glBindVertexArray(LineVAO);
    glDrawArrays(GL_LINE_STRIP, 0, 2);
    glBindVertexArray(0);
}

