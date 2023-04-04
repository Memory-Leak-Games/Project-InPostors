#pragma once

#include "Renderable2D.h"

namespace mlg {

    class Image : public Renderable2D {
    private:
        static uint32_t rectVao, rectVbo;
        glm::vec2 size{1.f, 1.f};
        glm::vec2 position{0.f, 0.f};

    protected:
        std::shared_ptr<class MaterialAsset> material;
    public:
        explicit Image(const std::shared_ptr<struct MaterialAsset>& material);

        void InitializeRect();

        void Draw() override;

        const glm::vec2& GetSize() const;
        const glm::vec2& GetPosition() const;

        void SetSize(const glm::vec2& size);
        void SetPosition(const glm::vec2& position);

    protected:
        void DrawRect();
    };

} // mlg
