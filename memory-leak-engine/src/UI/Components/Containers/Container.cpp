#include "UI/Components/Containers/Container.h"

#include "UI/Components/UIComponent.h"
#include "UI/Components/UIFocusableComponent.h"

mlg::Container::Container(std::weak_ptr<Entity> owner, std::string name)
    : UIComponent(owner, name) {}

mlg::Container::~Container(){};

void mlg::Container::Start() {
    UpdateContainer();
    SetAnchor(anchor);
}

void mlg::Container::AddChild(std::weak_ptr<UIComponent> child) {
    children.push_front(child);
    BindToFocusedEvent(child);
    UpdateContainer();
}

void mlg::Container::RemoveChild(std::weak_ptr<UIComponent> child) {
    children.remove_if([child](std::weak_ptr<UIComponent> another) {
        return child.lock() == another.lock();
    });

    UpdateContainer();
}

void mlg::Container::SetRelativePosition(const glm::vec2& position) {
    UIComponent::SetRelativePosition(position);
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
    if (!focusedComponent.expired()) {
        focusedComponent.lock()->GrabFocus();
        return;
    }

    for (auto& child : std::ranges::reverse_view(children)) {
        if (TryFocusFocusableComponent(child.lock().get()))
            return;

        if (TryFocusContainer(child.lock().get()))
            return;
    }
}

void mlg::Container::SetVisible(bool visible) {
    for (auto& child : children) {
        child.lock()->SetVisible(visible);
    }
}

void mlg::Container::BindToFocusedEvent(const std::weak_ptr<UIComponent>& child) {
    auto childAsFocusable =
            std::dynamic_pointer_cast<UIFocusableComponent>(child.lock());

    if (childAsFocusable == nullptr)
        return;

    childAsFocusable->OnFocus.append([this, childAsFocusable]() {
        focusedComponent = childAsFocusable;
    });
}

bool mlg::Container::TryFocusFocusableComponent(UIComponent* child) {
    auto childAsFocusable =
            dynamic_cast<UIFocusableComponent*>(child);

    if (childAsFocusable == nullptr)
        return false;

    childAsFocusable->GrabFocus();
    return true;
}

bool mlg::Container::TryFocusContainer(UIComponent* child) {
    auto childAsContainer =
            dynamic_cast<Container*>(child);

    if (childAsContainer == nullptr)
        return false;

    childAsContainer->GrabFocus();
    return true;
}
