#include <utility>

#include "Core/HID/Input.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/UIComponent.h"
#include "UI/Components/UIFocusableComponent.h"
#include "include/UI/Components/Button.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"
#include "UI/UIRenderer.h"

#include "Gameplay/Entity.h"
#include "UI/Components/Label.h"

namespace mlg {
    Button::Button(
            std::weak_ptr<Entity> owner, std::string name,
            const std::shared_ptr<MaterialAsset>& defaultMaterial,
            const std::shared_ptr<MaterialAsset>& focusMaterial,
            const std::shared_ptr<FontAsset>& font)
        : UIFocusableComponent(std::move(owner), name),
          defaultMaterial(defaultMaterial), focusMaterial(focusMaterial) {
            label = GetOwner().lock()->AddComponent<Label>( name + "_label", font);
            label.lock()->SetText("Button");
            label.lock()->SetAutoRegister(false);
          }

    void Button::GrabFocus() {
        UIFocusableComponent::GrabFocus();
    }

    void Button::Start() {
        thisAsRenderable = std::dynamic_pointer_cast<UIRenderable>(
                ComponentManager::GetByRawPointer(this).lock());

        UIRenderer::GetInstance()->AddRenderable(thisAsRenderable);
        UIRenderer::GetInstance()->AddRenderable(label);
    }

    void Button::Update() {
        UIFocusableComponent::Update();

        if (hasFocus && GetActive() && mlg::Input::IsActionJustPressed("ui_accept"))
            OnClick();
    }

    void Button::Draw(const UIRenderer* renderer) {
        ZoneScopedN("Draw Button");
        if (!visible)
            return;
        UIComponent::Draw(renderer);

        MaterialAsset* material;
        if (hasFocus)
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

    const std::weak_ptr<Label>& Button::GetLabel() const {
        return label;
    }

    void Button::SetVisible(bool visible) {
        UIComponent::SetVisible(visible);
        label.lock()->SetVisible(visible);
    }

    void Button::SetPosition(const glm::vec2& position) {
        UIComponent::SetPosition(position);
        label.lock()->SetPosition(position);
    }

    void Button::SetAnchor(const glm::vec2& anchor) {
        UIComponent::SetAnchor(anchor);
        label.lock()->SetAnchor(anchor);
    }

}// namespace mlg