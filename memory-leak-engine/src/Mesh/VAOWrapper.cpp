#include "include/Mesh/VAOWrapper.h"

using namespace mlg;

VAOWrapper::~VAOWrapper() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

VAOWrapper::VAOWrapper(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices,
                       const std::vector<Texture> &textures) {
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

    indicesCount = indices.size();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoord));


    glBindVertexArray(0);
}

void VAOWrapper::Draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

GLuint VAOWrapper::GetVaoId() const {
    return vao;
}

GLsizei VAOWrapper::GetIndicesCount() const {
    return indicesCount;
}

