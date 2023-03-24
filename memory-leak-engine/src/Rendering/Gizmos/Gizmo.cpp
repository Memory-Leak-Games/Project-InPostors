#include "Rendering//Gizmos/Gizmo.h"

using namespace mlg;

GLuint Gizmo::VAO;
GLuint Gizmo::VBO;
GLuint Gizmo::EBO;
std::shared_ptr<ShaderWrapper> Gizmo::Shader;

void Gizmo::Initialize() {
    if (VAO != 0)
        return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *) 0);

    glBindVertexArray(0);

    Shader = std::make_shared<ShaderWrapper>("res/shaders/gizmos.vert", "res/shaders/gizmos.frag");
}
