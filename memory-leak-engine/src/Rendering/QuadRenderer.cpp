#include "Rendering/QuadRenderer.h"
#include "glad/glad.h"

#include "Macros.h"

namespace mlg {
    QuadRenderer::QuadRenderer()
    : vao(0), vbo(0) {}

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

        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);

        glNamedBufferData(vbo, quadVertices.size() * sizeof(float), &quadVertices[0], GL_STATIC_DRAW);

        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);

        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribBinding(vao, 1, 0);
        glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 4 * sizeof(float));
    }

} // mlg