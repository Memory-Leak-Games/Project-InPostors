#include "Rendering/Assets/TextureAsset.h"

#include "stb_image.h"

#include "Macros.h"
#include "glad/glad.h"

namespace mlg {
    TextureAsset::TextureAsset(const std::string& path) : Asset(path) {}

    void TextureAsset::Load() {
        std::string path = GetPath();
        SPDLOG_DEBUG("Loading texture at path: {}", path);

        int width, height, numberOfComponents;
        stbi_set_flip_vertically_on_load(true);
        uint8_t* imageData = stbi_load(path.c_str(), &width, &height, &numberOfComponents, 0);
        if (imageData) {
            int32_t storageColorFormat;
            int32_t imageColorFormat;
            switch (numberOfComponents) {
                case 1:
                    imageColorFormat = GL_RED;
                    storageColorFormat = GL_RED;
                    break;
                case 3:
                    imageColorFormat = GL_RGB;
                    storageColorFormat = GL_RGB8;
                    break;
                case 4:
                    imageColorFormat = GL_RGBA;
                    storageColorFormat = GL_RGBA8;
                    break;
                default:
                    MLG_ASSERT_MSG(false, "Texture format not implemented");
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

            glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureStorage2D(textureID, 1, storageColorFormat, width, height);
            glTextureSubImage2D(textureID, 0, 0, 0, width, height, imageColorFormat, GL_UNSIGNED_BYTE, imageData);
            glGenerateTextureMipmap(textureID);

            stbi_image_free(imageData);
        } else {
            SPDLOG_ERROR("Failed to load texture at path: {}", path);
            stbi_image_free(imageData);
        }
    }

    TextureAsset::~TextureAsset() {
        glDeleteTextures(1, &textureID);
    }

    void TextureAsset::Bind() {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

}// namespace mlg