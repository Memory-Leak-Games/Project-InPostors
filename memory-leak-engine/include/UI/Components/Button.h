#pragma once

#include "UIFocusableComponent.h"

namespace mlg {
    class Button : public UIFocusableComponent {
    private:
        glm::vec2 size{200.f, 50.f};
        std::shared_ptr<class MaterialAsset> defaultMaterial;
        std::shared_ptr<class MaterialAsset> focusMaterial;

    public:
        Button(std::weak_ptr<Entity> owner, std::string name,
               const std::shared_ptr<struct MaterialAsset>& defaultMaterial,
               const std::shared_ptr<struct MaterialAsset>& focusMaterial);
        void Draw(const class mlg::UIRenderer* renderer) override;
        void GrabFocus() override;

        [[nodiscard]] const glm::vec2& GetSize() const;
        void SetSize(const glm::vec2& size);
    };
}// namespace mlg