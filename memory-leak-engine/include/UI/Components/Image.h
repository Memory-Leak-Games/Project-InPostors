#pragma once

#include "UIComponent.h"

namespace mlg {

    class Image : public UIComponent {
    private:
        static uint32_t rectVao, rectVbo;
        glm::vec2 size{1.f, 1.f};

    protected:
        std::shared_ptr<class MaterialAsset> material;
    public:
        explicit Image(std::weak_ptr<Entity> owner, std::string name,
                       const std::shared_ptr<struct MaterialAsset>& material);

        void InitializeRect();

        void Draw(const class Renderer2D* renderer) override;

        [[nodiscard]] const glm::vec2& GetSize() const;

        void SetSize(const glm::vec2& size);

    protected:
        void DrawRect();
    };

} // mlg
