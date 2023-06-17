#pragma once

#include "UI/Components/Button.h"
#include "UI/Components/Containers/Container.h"

namespace mlg {

    class ImageButton : public Button {
    private:
        std::weak_ptr<class Container> container;
        std::weak_ptr<class Image> image;

    public:
        ImageButton(std::weak_ptr<Entity> owner, std::string name,
                    const std::shared_ptr<MaterialAsset>& defaultMaterial,
                    const std::shared_ptr<MaterialAsset>& focusMaterial,
                    const std::shared_ptr<FontAsset>& font,
                    const std::shared_ptr<MaterialAsset>& imageMaterial);

        void Start() override;

        void SetVisible(bool visible) override;
        void SetRelativePosition(const glm::vec2& position) override;
        void SetPosition(const glm::vec2& position) override;
        void SetAnchor(const glm::vec2& anchor) override;

        [[nodiscard]] const std::weak_ptr<Image>& GetImage() const;
    };


}// namespace mlg