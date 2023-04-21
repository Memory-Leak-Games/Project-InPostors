#include "include/UI/Components/Image.h"

#include "Macros.h"
#include "UI/Renderer2D.h"

#include "glad/glad.h"

#include "include/Rendering/Assets/MaterialAsset.h"
#include "include/Rendering/ShaderProgram.h"

#include <utility>

namespace mlg {
    uint32_t Image::rectVao;
    uint32_t Image::rectVbo;

    Image::Image(std::weak_ptr<Entity> owner, std::string name, const std::shared_ptr<struct MaterialAsset>& material)
        : UIComponent(std::move(owner), std::move(name)), material(material) {
        if (rectVao == 0 || rectVbo == 0)
            InitializeRect();
    }

    void Image::InitializeRect() {
        static const std::vector<float> rectVertices({
                                                             -0.5f, 0.5f,
                                                             -0.5f, -0.5f,
                                                             0.5f, -0.5f,

                                                             -0.5f, 0.5f,
                                                             0.5f, -0.5f,
                                                             0.5f, 0.5f
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

    void Image::Draw(const Renderer2D* renderer) {
        UIComponent::CalculateActualPosition(renderer);

        material->Activate();

        material->GetShaderProgram()->SetVec2F("size", size * renderer->uiScale);
        material->GetShaderProgram()->SetVec2F("screenPosition", actualPosition);
        material->GetShaderProgram()->SetMat4F("projection", renderer->GetProjection());

        DrawRect();

        material->DeActivate();

//        ImGui::Begin("image anchor");
//        ImGui::Text("uiScale: %f", renderer->uiScale);
//        ImGui::Text("actualPosition: %f, %f", actualPosition.x, actualPosition.y);
//        ImGui::SliderFloat2("anchor", (float*)&anchor, 0.0f, 1.0f);
//        ImGui::End();
    }

    void Image::DrawRect() {
        MLG_ASSERT(rectVao != 0);

        glBindVertexArray(rectVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    const glm::vec2& Image::GetSize() const {
        return size;
    }

    void Image::SetSize(const glm::vec2& size) {
        Image::size = size;
    }
} // mlg