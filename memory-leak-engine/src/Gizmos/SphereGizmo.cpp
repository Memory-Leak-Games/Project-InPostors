#include "Gizmos/SphereGizmo.h"

#include <vector>

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>

void SphereGizmo::Draw(glm::vec3 Position, float Radius, uint16_t LOD, glm::vec4 Color)
{
    int i, j;
    std::vector<GLfloat> Vertices;
    std::vector<GLuint> Indices;
    int indicator = 0;
    for(i = 0; i <= LOD; i++)
    {
        double lat0 = glm::pi<double>() * (-0.5 + (double) (i - 1) / LOD);
        double z0 = glm::sin(lat0);
        double zr0 = glm::cos(lat0);

        double lat1 = glm::pi<double>() * (-0.5 + (double) i / LOD);
        double z1 = glm::sin(lat1);
        double zr1 = glm::cos(lat1);

        for (j = 0; j <= LOD; j++)
        {
            double lng = 2 * glm::pi<double>() * (double) (j - 1) / LOD;
            double x = glm::cos(lng);
            double y = glm::sin(lng);

            Vertices.push_back(x * zr0 * Radius + Position.x);
            Vertices.push_back(y * zr0 * Radius + Position.y);
            Vertices.push_back(z0 * Radius + Position.z);
            Indices.push_back(indicator);
            indicator++;

            Vertices.push_back(x * zr1 * Radius + Position.x);
            Vertices.push_back(y * zr1 * Radius + Position.y);
            Vertices.push_back(z1 * Radius + Position.z);
            Indices.push_back(indicator);
            indicator++;
        }
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GLfloat), &Vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), &Indices[0], GL_DYNAMIC_DRAW);

    Shader->Activate();
    Shader->SetVec4F("Color", Color);

    glDrawElements(GL_LINE_STRIP, Indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
