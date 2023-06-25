#pragma once

#include "UI/Components/Image.h"

namespace mlg {

    class DynamicImage : public Image {
    public:
        explicit DynamicImage(std::weak_ptr<Entity> owner, std::string name);

        void SetTexture(const std::shared_ptr<class TextureAsset>& texture);
    };

}// namespace mlg