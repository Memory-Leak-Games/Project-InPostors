#include "include/Mesh/Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& Vertices, const std::vector<GLuint>& Indices,
           const std::vector<Texture>& Textures) : vertices(Vertices), indices(Indices), textures(Textures),
                                                   vao(Vertices, Indices, Textures)
{
}

void Mesh::Draw(ShaderWrapper& Shader) const
{
    BindTextures(Shader);

    vao.Draw();
}

void Mesh::BindTextures(const ShaderWrapper& Shader) const
{
    uint16_t TextureIndex = 0;
    uint16_t DiffuseIndex = 0;
    uint16_t SpecularIndex = 0;

    for (const Texture & Item : textures)
    {
        glActiveTexture(GL_TEXTURE0 + TextureIndex);

        std::string Number;
        if(Item.textureType == "texture_diffuse")
            Number = std::to_string(DiffuseIndex++);
        else if(Item.textureType == "texture_specular")
            Number = std::to_string(SpecularIndex++);
        else if (Item.textureType == "texture_normalmap")
            Number = std::to_string(0);

        Shader.SetFloat(Item.textureType + Number, TextureIndex);
        glBindTexture(GL_TEXTURE_2D, Item.id);
        TextureIndex++;
    }
    glActiveTexture(GL_TEXTURE0);
}

const VAOWrapper& Mesh::GetVao() const
{
    return vao;
}
