#include "Rendering/Mesh.h"

#include "Core/AssetManager/TextureAsset.h"

using namespace mlg;

Mesh::Mesh(const std::vector<Vertex>& Vertices, const std::vector<GLuint>& Indices, const std::vector<Texture>& Textures)
: vertices(Vertices), indices(Indices), textures(Textures), vao(0), vbo(0), ebo(0) {
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

void Mesh::Draw(ShaderWrapper& Shader) const {
    BindTextures(Shader);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (int32_t) indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::BindTextures(const ShaderWrapper& Shader) const {
    uint16_t TextureIndex = 0;
    uint16_t DiffuseIndex = 0;
    uint16_t SpecularIndex = 0;

    for (const Texture& Item : textures) {
        glActiveTexture(GL_TEXTURE0 + TextureIndex);

        std::string Number;
        if (Item.textureType == "texture_diffuse")
            Number = std::to_string(DiffuseIndex++);
        else if (Item.textureType == "texture_specular")
            Number = std::to_string(SpecularIndex++);
        else if (Item.textureType == "texture_normalmap")
            Number = std::to_string(0);

        Shader.SetInt(Item.textureType + Number, TextureIndex);
        Item.textureAsset->Bind();
        TextureIndex++;
    }
    glActiveTexture(GL_TEXTURE0);
}
