#pragma once

#include "array"

namespace mlg {

    struct Neighbours {
        std::array<std::array<char, 3>, 3> tiles;

        const char& leftUp = tiles[0][0];
        const char& up = tiles[0][1];
        const char& rightUp = tiles[0][2];

        const char& left = tiles[1][0];
        const char& center = tiles[1][1];
        const char& right = tiles[1][2];

        const char& leftDown = tiles[2][0];
        const char& down = tiles[2][1];
        const char& rightDown = tiles[2][2];
    };

} // mlg
