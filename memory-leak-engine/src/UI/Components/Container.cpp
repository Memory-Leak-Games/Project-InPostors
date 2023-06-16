#include "UI/Components/Container.h"
#include "UI/Components/UIComponent.h"
#include "UI/Components/UIFocusableComponent.h"
#include <glm/fwd.hpp>

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

    return size + glm::vec2(padding);
}

void mlg::Container::GrabFocus() {
    for (auto& child : children) {
        std::weak_ptr<UIFocusableComponent> childAsFocusable =
                std::dynamic_pointer_cast<UIFocusableComponent>(child.lock());
        
        if (childAsFocusable.expired())
            continue;
        
        childAsFocusable.lock()->GrabFocus();
        break;
    }
}

void mlg::Container::SetVisible(bool visible) {
    for (auto& child : children) {
        child.lock()->SetVisible(visible);
    }
}


