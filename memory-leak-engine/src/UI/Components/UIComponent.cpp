#include "UI/Components/UIComponent.h"

#include "UI/UIRenderer.h"

namespace mlg {

    UIComponent::UIComponent(std::weak_ptr<Entity> owner, std::string name) : Component(std::move(owner), std::move(name)) {};

    void UIComponent::CalculateActualPosition(const UIRenderer* renderer) {
        actualPositionDirty = false;

        // Component's offset from anchor for 1280x720 window
        glm::vec2 defaultOffset = {
                position.x - 1280.f * anchor.x,
                position.y - 720.f * anchor.y
        };

        // Calculate anchors' current position
        glm::vec2 anchorPosition = {
                renderer->windowWidth * anchor.x,
                renderer->windowHeight * anchor.y
        };

        actualPosition = anchorPosition + defaultOffset * renderer->uiScale;
    }

    void UIComponent::Draw(const UIRenderer* renderer) {
        if (renderer->windowSizeDirty || actualPositionDirty)
        {
            CalculateActualPosition(renderer);
        }
    }

    const glm::vec2& UIComponent::GetPosition() const {
        return position;
    }

    const glm::vec2& UIComponent::GetAnchor() const {
        return anchor;
    }

    void UIComponent::SetPosition(const glm::vec2& position) {
        UIComponent::position = position;
        actualPositionDirty = true;
    }

    void UIComponent::SetAnchor(const glm::vec2& anchor) {
        UIComponent::anchor = glm::clamp(anchor, 0.0f, 1.0f);
        actualPositionDirty = false;
    }

}