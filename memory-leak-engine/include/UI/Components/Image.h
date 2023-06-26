#pragma once

#include "UIComponent.h"

namespace mlg {

    class Image : public UIComponent {
    private:
        glm::vec2 size{1.f, 1.f};

    protected:
        std::shared_ptr<class MaterialAsset> material;

    public:
        glm::vec4 tint = {1.0, 1.0, 1.0, 1.0};

        explicit Image(std::weak_ptr<Entity> owner, std::string name,
                       const std::shared_ptr<class MaterialAsset>& material);

        void Draw(const class UIRenderer* renderer) override;

        [[nodiscard]] glm::vec2 GetSize() const override;
        void SetSize(const glm::vec2& size);

        void SetMaterial(const std::shared_ptr<class MaterialAsset>& material);
    };

}// namespace mlg
