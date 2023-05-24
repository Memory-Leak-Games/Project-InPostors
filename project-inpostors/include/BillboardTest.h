#pragma once

#include "Rendering/Camera.h"
#include "UI/Components/Image.h"

namespace mlg {
    class MaterialAsset;
}

class BillboardTest : public mlg::Image {
public:
    std::weak_ptr<mlg::Entity> player;
    std::weak_ptr<mlg::Camera> camera;
    BillboardTest(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, const std::shared_ptr<mlg::MaterialAsset>& material);
    void Draw(const class mlg::UIRenderer *renderer) override;
};
