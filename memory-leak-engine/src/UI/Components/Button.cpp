#include <utility>

#include "include/UI/Components/Button.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "UI/UIRenderer.h"
#include "Rendering/ShaderProgram.h"

namespace mlg {
    Button::Button(std::weak_ptr<Entity> owner, std::string name,
                   const std::shared_ptr<MaterialAsset>& defaultMaterial,
                   const std::shared_ptr<MaterialAsset>& focusMaterial)
        : UIFocusableComponent(std::move(owner), name), defaultMaterial(defaultMaterial), focusMaterial(focusMaterial)
    {}

    void Button::GrabFocus() {
        UIFocusableComponent::GrabFocus();
    }

    void Button::Draw(const UIRenderer* renderer) {
        ZoneScopedN("Draw Button");
        if(!visible)
            return;
        UIComponent::Draw(renderer);

        MaterialAsset* material;
        if(hasFocus)
            material = focusMaterial.get();
        else
            material = defaultMaterial.get();

        material->Activate();
        material->GetShaderProgram()->SetVec2F("size", size * renderer->uiScale);
        material->GetShaderProgram()->SetVec2F("screenPosition", actualPosition);
        material->GetShaderProgram()->SetMat4F("projection", renderer->GetProjection());

        MLG_ASSERT(renderer->vao != 0);
        glBindVertexArray(renderer->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        material->DeActivate();
    }

    const glm::vec2& Button::GetSize() const {
        return size;
    }

    void Button::SetSize(const glm::vec2& size) {
        Button::size = size;
    }

}