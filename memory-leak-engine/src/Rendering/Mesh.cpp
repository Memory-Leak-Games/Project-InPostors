#include "Rendering/Mesh.h"

#include "include/Rendering/Assets/TextureAsset.h"

using namespace mlg;

Mesh::Mesh(const std::vector<Vertex>& Vertices, const std::vector<GLuint>& Indices)
: vertices(Vertices), indices(Indices), vao(0), vbo(0), ebo(0) {
    SetupBuffers();
}

void Mesh::SetupBuffers() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(vertices.size() * sizeof(Vertex)),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLint>(indices.size() * sizeof(GLuint)),
                 &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

void Mesh::Draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (int32_t) indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
