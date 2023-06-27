#pragma once

#include "UI/Components/Image.h"
#include <string>

namespace mlg {

    class DynamicImage : public Image {
    public:
        explicit DynamicImage(std::weak_ptr<Entity> owner, std::string name);

        void SetTexture(const std::shared_ptr<class TextureAsset>& texture);
        void SetTexture(const std::string& texturePath);
    };

}// namespace mlg