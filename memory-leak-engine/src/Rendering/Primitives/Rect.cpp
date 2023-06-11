#include "Rendering/Primitives/Rect.h"
#include "glad/glad.h"

#include "Macros.h"

namespace mlg {
    Rect::Rect()
        : vao(0), vbo(0) {
        static const std::vector<float> quadVertices({// positions   // texCoords
                                                      -1.f, 1.f, 0.f, 0.0f, 1.f,
                                                      -1.f, -1.f, 0.f, 0.0f, 0.f,
                                                      1.f, -1.f, 0.f, 1.0f, 0.f,

                                                      -1.f, 1.f, 0.f, 0.f, 1.f,
                                                      1.f, -1.f, 0.f, 1.f, 0.f,
                                                      1.f, 1.f, 0.f, 1.f, 1.f});

        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);

        glNamedBufferStorage(vbo, quadVertices.size() * sizeof(float), &quadVertices[0], 0);

        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribBinding(vao, 1, 0);
        glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 5 * sizeof(float));
    }

    Rect::~Rect() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void Rect::Draw() {
        MLG_ASSERT(vao != 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    uint32_t Rect::GetVao() {
        return vao;
    }

    size_t Rect::GetSize() {
        return 6;
    }

}// namespace mlg