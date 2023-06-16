#pragma once

#include "UIFocusableComponent.h"
#include <memory>

namespace mlg {
    class Button : public UIFocusableComponent {
    private:
        glm::vec2 size{200.f, 50.f};
        std::shared_ptr<class MaterialAsset> defaultMaterial;
        std::shared_ptr<MaterialAsset> focusMaterial;

        std::weak_ptr<class Label> label;

    public:
        eventpp::CallbackList<void()> OnClick;

        Button(std::weak_ptr<Entity> owner, std::string name,
               const std::shared_ptr<MaterialAsset>& defaultMaterial,
               const std::shared_ptr<MaterialAsset>& focusMaterial,
               const std::shared_ptr<class FontAsset>& font);

        void Draw(const class mlg::UIRenderer* renderer) override;
        void GrabFocus() override;

        void Start() override;
        void Update() override;

        [[nodiscard]] glm::vec2 GetSize() const override;
        void SetSize(const glm::vec2& size);

        [[nodiscard]] const std::weak_ptr<Label>& GetLabel() const;

        void SetVisible(bool visible) override;
        void SetPosition(const glm::vec2& position) override;
        void SetAnchor(const glm::vec2& anchor) override;
    };
}// namespace mlg