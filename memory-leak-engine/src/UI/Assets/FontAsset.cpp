#include "UI/Assets/FontAsset.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Macros.h"
#include "glad/glad.h"

mlg::FontAsset::FontAsset(const std::string& path) : Asset(path) {}

//TODO: Implement me
mlg::FontAsset::~FontAsset() {
}

void mlg::FontAsset::Load() {

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        SPDLOG_ERROR("Could not init FreeType Library");

    FT_Face face;
    if (FT_New_Face(ft, "res/fonts/comic.ttf", 0, &face))
        SPDLOG_ERROR("Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (uint8_t c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            SPDLOG_ERROR("Failed to load Glyph");
            continue;
        }

//         Generate texture
        uint32_t texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);

        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        auto width = (int32_t) face->glyph->bitmap.width;
        auto height = (int32_t) face->glyph->bitmap.rows;

        if (width == 0 || height == 0)
            continue;

        glTextureStorage2D(texture, 1, GL_R8, width, height);
        glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glm::vec4 borderColor{0.f};
        glTextureParameterfv(texture, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));

        // Now store character for later use
        Character character {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
        };

        characters.insert(std::pair<char8_t, Character>(c, character));

    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}