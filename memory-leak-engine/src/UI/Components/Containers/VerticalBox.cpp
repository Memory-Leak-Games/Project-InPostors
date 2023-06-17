#include "UI/Components/Containers/VerticalBox.h"
#include "UI/Components/UIFocusableComponent.h"

mlg::VerticalBox::VerticalBox(std::weak_ptr<Entity> owner, std::string name)
    : Container(owner, name) {}

mlg::VerticalBox::~VerticalBox(){

};

void mlg::VerticalBox::UpdateContainer() {
    if (children.empty()) return;

    glm::vec2 position = GetPosition();
    glm::vec2 size = GetSize();
    glm::vec2 firstSize = children.front().lock()->GetSize();
    float y = -size.y / 2.0f + position.y;

    for (auto& child : children) {
        y += child.lock()->GetSize().y * 0.5f;
        child.lock()->SetRelativePosition(glm::vec2{position.x, y});
        y += child.lock()->GetSize().y * 0.5f;
    }

    UpdateFocusableComponents();
}

void mlg::VerticalBox::UpdateFocusableComponents() {
    std::vector<std::weak_ptr<UIFocusableComponent>> focusableComponents;

    for (auto& child : children) {
        std::weak_ptr<UIFocusableComponent> childAsFocusable =
                std::dynamic_pointer_cast<UIFocusableComponent>(child.lock());

        if (childAsFocusable.expired())
            continue;

        focusableComponents.push_back(childAsFocusable);
    }

    for (int i = 0; i < focusableComponents.size(); i++) {
        focusableComponents[i].lock()->next.bottom =
                focusableComponents[(i - 1) % focusableComponents.size()];
        focusableComponents[i].lock()->next.top =
                focusableComponents[(i + 1) % focusableComponents.size()];
    }
}
