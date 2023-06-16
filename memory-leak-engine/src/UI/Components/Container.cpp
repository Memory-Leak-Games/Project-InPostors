#include "UI/Components/Container.h"
#include "UI/Components/UIComponent.h"

mlg::Container::Container(std::weak_ptr<Entity> owner, std::string name)
: UIComponent(owner, name) { }

mlg::Container::~Container() { };

void mlg::Container::Start() {
    UpdateContainer();
    SetAnchor(anchor);
}

void mlg::Container::AddChild(std::weak_ptr<UIComponent> child) {
    children.push_back(child);
    UpdateContainer();
}

void mlg::Container::RemoveChild(std::weak_ptr<UIComponent> child) {
    children.remove_if([child](std::weak_ptr<UIComponent> another){
        return child.lock() == another.lock();
    });

    UpdateContainer();
}

void mlg::Container::SetPosition(const glm::vec2& position) {
    UIComponent::SetPosition(position);
    UpdateContainer();
}

void mlg::Container::SetAnchor(const glm::vec2& anchor) {
    UIComponent::SetAnchor(anchor);

    for (auto& child : children) {
        child.lock()->SetAnchor(anchor);
    }    
}

glm::vec2 mlg::Container::GetSize() const {
    glm::vec2 size;

    for (auto& child : children) {
        glm::vec2 childSize = child.lock()->GetSize();
        size.x = std::max(size.x, childSize.x);
        size.y += childSize.y;
    }    

    return size;
}


