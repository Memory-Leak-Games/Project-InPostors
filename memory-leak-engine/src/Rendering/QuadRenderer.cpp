#include "Rendering/QuadRenderer.h"
#include "glad/glad.h"

#include "Macros.h"

namespace mlg {
    QuadRenderer::QuadRenderer()
    : vao(0), vbo(0) {
    }

    QuadRenderer::~QuadRenderer() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void QuadRenderer::Draw() {
        MLG_ASSERT(vao != 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void QuadRenderer::Initialize() {
        // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        static const std::vector<float> quadVertices({
                                                             // positions   // texCoords
                                                             -1.0f, 1.0f, 0.0f, 1.0f,
                                                             -1.0f, -1.0f, 0.0f, 0.0f,
                                                             1.0f, -1.0f, 1.0f, 0.0f,

                                                             -1.0f, 1.0f, 0.0f, 1.0f,
                                                             1.0f, -1.0f, 1.0f, 0.0f,
                                                             1.0f, 1.0f, 1.0f, 1.0f
                                                     });

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, (int32_t) (quadVertices.size() * sizeof(float)), &quadVertices[0],
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));

        glBindVertexArray(0);
    }

} // mlg