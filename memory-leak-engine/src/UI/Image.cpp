#include "UI/Image.h"
#include "Macros.h"
#include "Core/Window.h"

#include <glad/glad.h>

#include <Rendering/Assets/MaterialAsset.h>
#include <Rendering/ShaderProgram.h>

namespace mlg {
    uint32_t Image::rectVao;
    uint32_t Image::rectVbo;

    Image::Image(const std::shared_ptr<struct MaterialAsset>& material) : material(material) {
        if (rectVao == 0 || rectVbo == 0)
            InitializeRect();
    }

    void Image::InitializeRect() {
        static const std::vector<float> rectVertices({
                                                             0.f, 1.0f,
                                                             0.f, 0.f,
                                                             1.0f, 0.f,

                                                             0.f, 1.0f,
                                                             1.0f, 0.f,
                                                             1.0f, 1.0f
                                                     });

        glCreateVertexArrays(1, &rectVao);
        glCreateBuffers(1, &rectVbo);

        glNamedBufferData(rectVbo, sizeof(float) * 6 * 4, &rectVertices[0], GL_DYNAMIC_DRAW);

        glCreateVertexArrays(1, &rectVao);
        glCreateBuffers(1, &rectVbo);

        glNamedBufferData(rectVbo, rectVertices.size() * sizeof(float), &rectVertices[0], GL_STATIC_DRAW);

        glEnableVertexArrayAttrib(rectVao, 0);
        glVertexArrayAttribBinding(rectVao, 0, 0);
        glVertexArrayAttribFormat(rectVao, 0, 2, GL_FLOAT, GL_FALSE, 0);

        glVertexArrayVertexBuffer(rectVao, 0, rectVbo, 0, 2 * sizeof(float));
    }

    void Image::Draw() {
        material->Activate();

        // TODO: nie liczyć tego co klatkę tylko jeżeli wielkość okna się zmieni (może trzymać to w Rendererz jako pole statyczne)
        Window* window = Window::GetInstance();
        glm::mat4 projection = glm::ortho(0.0f, (float) window->GetWidth(), 0.0f, (float) window->GetHeight());

        material->GetShaderProgram()->SetVec2F("size", size);
        material->GetShaderProgram()->SetVec2F("screenPosition", position);
        material->GetShaderProgram()->SetMat4F("projection", projection);

        MLG_ASSERT(rectVao != 0);

        glBindVertexArray(rectVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        material->DeActivate();
    }

    const glm::vec2& Image::GetSize() const {
        return size;
    }

    void Image::SetSize(const glm::vec2& size) {
        Image::size = size;
    }

    const glm::vec2& Image::GetPosition() const {
        return position;
    }

    void Image::SetPosition(const glm::vec2& position) {
        Image::position = position;
    }
} // mlg