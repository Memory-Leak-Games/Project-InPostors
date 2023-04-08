#include "UI/Components/UIComponent.h"

namespace mlg {

    UIComponent::UIComponent(std::weak_ptr<Entity> owner, std::string name) : Component(std::move(owner), std::move(name)) {};

    const glm::vec2& UIComponent::GetPosition() const {
        return position;
    }

    void UIComponent::SetPosition(const glm::vec2& position) {
        UIComponent::position = position;
    }

}