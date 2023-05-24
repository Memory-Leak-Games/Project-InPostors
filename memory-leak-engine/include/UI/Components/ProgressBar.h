#pragma once

#include "Image.h"

namespace mlg {

    class ProgressBar: public Image {
    private:
        float percentage = 0.f;
    public:
        explicit ProgressBar(std::weak_ptr<Entity> owner, std::string name,
                             const std::shared_ptr<struct MaterialAsset>& material);

        void Draw(const UIRenderer* renderer) override;
    };

} // mlg
