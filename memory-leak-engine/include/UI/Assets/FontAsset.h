#pragma once

#include "Core/AssetManager/Asset.h"

namespace mlg {

    class FontAsset : public Asset {
    public:
        struct Character {
            uint32_t   textureID;  // ID handle of the glyph texture
            glm::ivec2 Size;       // Size of glyph
            glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
            int64_t   Advance;    // Offset to advance to next glyph
        };

        //TODO: Load font size
        int fontSize = 32;
        std::map<char8_t, Character> characters;

        explicit FontAsset(const std::string& path);
        void Load() override;
    };

}
