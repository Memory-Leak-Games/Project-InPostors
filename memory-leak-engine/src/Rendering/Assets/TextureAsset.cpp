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
            int32_t colorFormat;
            switch (numberOfComponents) {
                case 1:
                    colorFormat = GL_RED;
                    break;
                case 3:
                    colorFormat = GL_RGB;
                    break;
                case 4:
                    colorFormat = GL_RGBA;
                    break;
                default:
                    MLG_ASSERT(false, "Texture format not implemented");
            }

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, imageData);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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