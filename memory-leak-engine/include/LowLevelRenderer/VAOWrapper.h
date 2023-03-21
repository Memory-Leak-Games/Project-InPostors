#pragma once

#include <glad/glad.h>

namespace mlg {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    struct Texture {
        std::shared_ptr<class TextureAsset> textureAsset;
        std::string textureType;
        std::string texturePath;
    };

    class VAOWrapper {
    private:
        GLuint vao;
        GLuint vbo;
        GLuint ebo;

        GLsizei indicesCount;
    public:
        VAOWrapper(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices,
                   const std::vector<Texture> &textures);

        ~VAOWrapper();

        void Draw() const;

        GLuint GetVaoId() const;

        GLsizei GetIndicesCount() const;
    };
}
