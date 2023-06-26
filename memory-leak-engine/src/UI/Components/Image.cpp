#include "UI/Components/Image.h"

#include "Macros.h"
#include "UI/UIRenderer.h"

#include "glad/glad.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"

namespace mlg {
    Image::Image(std::weak_ptr<Entity> owner, std::string name, const std::shared_ptr<MaterialAsset>& material)
        : UIComponent(std::move(owner), std::move(name)), material(material) {
    }

    void Image::Draw(const UIRenderer* renderer) {
        ZoneScopedN("Draw Image");
        UIComponent::Draw(renderer);
        if(!visible)
            return;

        glEnable(GL_BLEND);
        material->Activate();

        material->GetShaderProgram()->SetVec2F("size", size * renderer->uiScale);
        material->GetShaderProgram()->SetVec2F("screenPosition", actualPosition);
        material->GetShaderProgram()->SetMat4F("projection", renderer->GetProjection());
        if(tint != glm::vec4(1.0))
            material->GetShaderProgram()->SetVec4F("tint", tint);

        MLG_ASSERT(UIRenderer::GetInstance()->vao != 0);
        glBindVertexArray(UIRenderer::GetInstance()->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        material->DeActivate();
    }

    glm::vec2 Image::GetSize() const {
        return size + glm::vec2(2.0f * padding);
    }

    void Image::SetSize(const glm::vec2& size) {
        Image::size = size;
    }

    void Image::SetMaterial(const std::shared_ptr<class MaterialAsset>& material) {
        this->material = material;
    }
} // mlg