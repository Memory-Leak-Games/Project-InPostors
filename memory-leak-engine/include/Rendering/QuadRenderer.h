#pragma once

namespace mlg {

    class QuadRenderer {
        uint32_t vao;
        uint32_t vbo;
    public:
        QuadRenderer();
        ~QuadRenderer();

        void Activate();
        void Draw();
        void DeActivate();
    };

} // mlg
