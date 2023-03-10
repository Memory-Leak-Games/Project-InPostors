#include <vector>
#include "Gizmos/Arrow.h"
#include "ShaderWrapper.h"

void Arrow::Draw(glm::vec3 Start, glm::vec3 End, glm::vec4 Color)
{
    glm::vec3 Direction = glm::normalize(End - Start);

    glm::vec3 RightVector;

    if (std::abs(Direction.y) != 1.f)
        RightVector = glm::cross(Direction, glm::vec3(0.f, 0.5f, 0.f));
    else
        RightVector = glm::vec3(0.5f, 0.0f, 0.f);

    glm::vec3 TipOffset = -Direction * 0.5f;
    glm::vec3 TipLeft = End - RightVector + TipOffset;
    glm::vec3 TipRight = End + RightVector + TipOffset;

    std::vector<glm::vec3> Vertices = {Start, End, TipLeft, TipRight};
    std::vector<GLuint> Indices = {0, 1, 2, 3, 1};

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec3), &Vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(int), &Indices[0], GL_DYNAMIC_DRAW);

    Shader->Activate();
    Shader->SetVec4F("Color", Color);

    glDrawElements(GL_LINE_STRIP, Indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

