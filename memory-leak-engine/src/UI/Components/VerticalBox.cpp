#include "UI/Components/VerticalBox.h"
#include <glm/fwd.hpp>

mlg::VerticalBox::VerticalBox(std::weak_ptr<Entity> owner, std::string name)
    : Container(owner, name) { }

mlg::VerticalBox::~VerticalBox() {

};

void mlg::VerticalBox::UpdateContainer() {
    if (children.empty()) return;

    glm::vec2 position = GetPosition();
    glm::vec2 size = GetSize();
    glm::vec2 firstSize = children.front().lock()->GetSize();
    float y = -size.y / 2.0f + firstSize.y / 2.0f;

    for (auto& child : children) {
        child.lock()->SetPosition(position + glm::vec2{0.0f, y});
        y += child.lock()->GetSize().y;
    }
}
