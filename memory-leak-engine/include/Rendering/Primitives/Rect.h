#pragma once

namespace mlg {

    class Rect {
        uint32_t vao;
        uint32_t vbo;

    public:
        Rect();
        ~Rect();

        uint32_t GetVao();
        size_t GetSize();
    };

} // mlg
