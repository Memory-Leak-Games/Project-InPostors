#pragma once

#include "ShaderProgram.h"

namespace mlg {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    struct Texture {
        std::shared_ptr<class TextureAsset> textureAsset;
        std::string textureType;
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
        Mesh(const std::vector<Vertex> &Vertices, const std::vector<GLuint> &Indices);

        void Draw() const;

    private:
        void SetupBuffers();
    };
}
