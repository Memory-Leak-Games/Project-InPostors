#pragma once

#include "Image.h"

namespace mlg {

    class ProgressBar: public Image {
    private:
        float percentage = 0.f;
    public:
        explicit ProgressBar(const std::shared_ptr<struct MaterialAsset>& material);

        void Draw(const Renderer2D* renderer) override;
    };

} // mlg
