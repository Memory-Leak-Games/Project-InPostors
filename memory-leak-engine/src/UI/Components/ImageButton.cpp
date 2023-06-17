#include "UI/Components/ImageButton.h"

#include "Gameplay/Entity.h"
#include "UI/Components/Button.h"
#include "UI/Components/Containers/Container.h"
#include "UI/Components/Containers/VerticalBox.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "UI/UIRenderer.h"

mlg::ImageButton::ImageButton(std::weak_ptr<Entity> owner, std::string name,
                              const std::shared_ptr<MaterialAsset>& defaultMaterial,
                              const std::shared_ptr<MaterialAsset>& focusMaterial,
                              const std::shared_ptr<FontAsset>& font,
                              const std::shared_ptr<MaterialAsset>& imageMaterial)
    : Button(owner, name, defaultMaterial, focusMaterial, font) {
    image = owner.lock()->AddComponent<Image>(name + "Image", imageMaterial);
    image.lock()->SetAutoRegister(false);

    container = owner.lock()->AddComponent<VerticalBox>(name + "Container");
    container.lock()->AddChild(image);
    container.lock()->AddChild(GetLabel());
}

void mlg::ImageButton::Start() {
    Button::Start();
    UIRenderer::GetInstance()->AddRenderable(image);
}

void mlg::ImageButton::SetVisible(bool visible) {
    Button::SetVisible(visible);
    container.lock()->SetVisible(visible);
}

void mlg::ImageButton::SetRelativePosition(const glm::vec2& position) {
    Button::SetRelativePosition(position);
    container.lock()->SetRelativePosition(position);
}

void mlg::ImageButton::SetPosition(const glm::vec2& position) {
    Button::SetPosition(position);
    container.lock()->SetPosition(position);
}

void mlg::ImageButton::SetAnchor(const glm::vec2& anchor) {
    container.lock()->SetAnchor(anchor);
}

const std::weak_ptr<mlg::Image>& mlg::ImageButton::GetImage() const {
    return image;
}

