#pragma once

#include "ShaderProgram.h"

namespace mlg {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    class Mesh {
    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

    private:
        uint32_t vao;
        uint32_t vbo;
        uint32_t ebo;
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);
        ~Mesh();

        void Draw() const;

    private:
        void SetupBuffers();
    };
}
