#pragma once

#include "UIComponent.h"

namespace mlg {

    class Image : public UIComponent {
    private:
        glm::vec2 size{1.f, 1.f};

    public:
        std::shared_ptr<class MaterialAsset> material;
        glm::vec4 tint = {1.0, 1.0, 1.0, 1.0};

        explicit Image(std::weak_ptr<Entity> owner, std::string name,
                       const std::shared_ptr<class MaterialAsset>& material);

        void Draw(const class UIRenderer* renderer) override;

        [[nodiscard]] const glm::vec2& GetSize() const;

        void SetSize(const glm::vec2& size);
    };

} // mlg
