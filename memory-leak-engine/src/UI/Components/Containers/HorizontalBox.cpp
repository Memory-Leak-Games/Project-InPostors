#include "UI/Components/Containers/HorizontalBox.h"
#include "UI/Components/UIFocusableComponent.h"
#include <spdlog/spdlog.h>

mlg::HorizontalBox::HorizontalBox(
        std::weak_ptr<Entity> owner, const std::string& name)
    : Container(std::move(owner), name) {}

mlg::HorizontalBox::~HorizontalBox() = default;

void mlg::HorizontalBox::UpdateContainer() {
    if (children.empty()) return;

    glm::vec2 position = GetPosition();
    glm::vec2 size = GetSize();
    float x = size.x / 2.0f + position.x;

    for (auto& child : children) {
        x -= child.lock()->GetSize().x * 0.5f;
        child.lock()->SetRelativePosition(glm::vec2{x, position.y});
        x -= child.lock()->GetSize().x * 0.5f;
    }

    UpdateFocusableComponents();
}

glm::vec2 mlg::HorizontalBox::GetSize() const {
    glm::vec2 size {0.f};

    for (auto& child : children) {
        glm::vec2 childSize = child.lock()->GetSize();
        size.x += childSize.x;
        size.y = std::max(size.y, childSize.y);
    }

    return size + glm::vec2(padding);
}

void mlg::HorizontalBox::UpdateFocusableComponents() {
    std::vector<std::weak_ptr<UIFocusableComponent>> focusableComponents;

    for (auto& child : children) {
        std::weak_ptr<UIFocusableComponent> childAsFocusable =
                std::dynamic_pointer_cast<UIFocusableComponent>(child.lock());

        if (childAsFocusable.expired())
            continue;

        focusableComponents.push_back(childAsFocusable);
    }

    for (int i = 0; i < focusableComponents.size(); i++) {
        focusableComponents[i].lock()->next.left =
                focusableComponents[(i + 1) % focusableComponents.size()];
        focusableComponents[i].lock()->next.right =
                focusableComponents[(i - 1) % focusableComponents.size()];
    }

    if (focusableComponents.size() > 1)
        focusableComponents.front().lock()->next.right = focusableComponents.back();
}
