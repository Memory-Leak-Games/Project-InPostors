#include "Rendering/Gizmos/Gizmos.h"

using namespace mlg;

GLuint Gizmos::LineVAO;
GLuint Gizmos::LineVBO;
GLuint Gizmos::BoxVAO;
GLuint Gizmos::BoxVBO;
GLuint Gizmos::BoxEBO;
std::shared_ptr<ShaderWrapper> Gizmos::Shader;

GLfloat LineVertices[]
{
    0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f
};

GLfloat BoxVertices[]
{
    0.5, 0.5, 0.5,
    0.5, 0.5, -0.5,
    -0.5, 0.5, -0.5,
    -0.5, 0.5, 0.5,
    0.5, -0.5, 0.5,
    0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5, -0.5, 0.5,
};

GLuint BoxIndices[]
{
    0, 1, 1, 2, 2, 3, 3, 0,
    4, 5, 5, 6, 6, 7, 7, 4,
    0, 4, 1, 5, 2, 6, 3, 7
};

void mlg::Gizmos::Initialize()
{
    // Load gizmo shader
    Shader = std::make_shared<ShaderWrapper>("res/shaders/gizmo.vert", "res/shaders/gizmo.frag");

    // Generate line gizmo
    {
        glGenBuffers(1, &LineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), LineVertices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &LineVAO);
        glBindVertexArray(LineVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Generate box gizmo
    {
        glGenBuffers(1, &BoxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, BoxVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), BoxVertices, GL_STATIC_DRAW);
        glGenBuffers(1, &BoxEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BoxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), BoxIndices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &BoxVAO);
        glBindVertexArray(BoxVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLuint), (void*) nullptr);
        glEnableVertexAttribArray(0);
    }

    // Unbind
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void mlg::Gizmos::Stop()
{

}

void mlg::Gizmos::DrawLine(glm::vec3 Start, glm::vec3 End, glm::vec4 Color)
{
    glm::mat4 World = glm::mat4(1.0f);
    World = glm::translate(World, Start);
    World = glm::scale(World, End - Start); // Use position diff to scale line (which is from [0, 0, 0] to [1, 1, 1])

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);

    glBindVertexArray(LineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Gizmos::DrawBox(Transform& Transform, glm::vec4 Color)
{
    const glm::mat4 World = Transform.GetLocalMatrix();

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);

    glBindVertexArray(BoxVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BoxEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Gizmos::DrawBox(glm::vec3 Position, glm::vec3 Size, glm::quat Rotation, glm::vec4 Color)
{
    glm::mat4 World = glm::mat4(1.0f);
    World = glm::translate(World, Position);
    World = World * glm::toMat4(Rotation);
    World = glm::scale(World, Size); // Default size is 1x1x1

    Shader->Activate();
    Shader->SetMat4F("World", World);
    Shader->SetVec4F("Color", Color);

    glBindVertexArray(BoxVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BoxEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
