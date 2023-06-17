#include "UI/Components/Containers/CanvasPanel.h"
#include <glm/fwd.hpp>

mlg::CanvasPanel::CanvasPanel(std::weak_ptr<Entity> owner, std::string name)
    : Container(owner, name) {}

mlg::CanvasPanel::~CanvasPanel() = default;

void mlg::CanvasPanel::SetSize(glm::vec2 size) {
    panelSize = size;
}

glm::vec2 mlg::CanvasPanel::GetSize() const {
    return panelSize;
}

void mlg::CanvasPanel::UpdateContainer() {
    if (children.empty()) return;

    glm::vec2 position = GetPosition();
    glm::vec2 size = GetSize();

    for (auto& child : children) {
        glm::vec2 childRelativePosition = child.lock()->GetRelativePosition();
        glm::vec2 childPosition = position + childRelativePosition - size * 0.5f;
        child.lock()->SetPosition(position);
    }
}