#include "UI/Components/UIComponent.h"

#include "Rendering/CommonUniformBuffer.h"
#include "UI/UIRenderer.h"
#include "Gameplay/Entity.h"

namespace mlg {

    UIComponent::UIComponent(std::weak_ptr<Entity> owner, std::string name) : Component(std::move(owner), std::move(name)) {}

    void UIComponent::CalculateActualPosition(const UIRenderer* renderer) {
        actualPositionDirty = false;

        // Component's offset from anchor for 1280x720 window
        glm::vec2 defaultOffset = {
                position.x - 1280.f * anchor.x,
                position.y - 720.f * anchor.y
        };

        // Calculate anchors' current position
        glm::vec2 anchorPosition = {
                (float)renderer->windowWidth * anchor.x,
                (float)renderer->windowHeight * anchor.y
        };

        actualPosition = anchorPosition + defaultOffset * renderer->uiScale;
    }

    void UIComponent::FollowTarget(const struct UIRenderer* renderer) {
        glm::vec4 world = glm::vec4(billboardTarget.lock()->GetTransform().GetWorldPosition(), 1.0f);
        auto projection =  mlg::CommonUniformBuffer::GetUniforms().projection;
        auto view =  mlg::CommonUniformBuffer::GetUniforms().view;
        actualPosition = projection * view * world;
        actualPosition += glm::vec2(1, 1);
        actualPosition.x *= (float)renderer->windowWidth * 0.5f;
        actualPosition.y *= (float)renderer->windowHeight * 0.5f;
        actualPosition += position * renderer->uiScale;
    }

    void UIComponent::Draw(const UIRenderer* renderer) {
        if(!isBillboard) {
            if (renderer->windowSizeDirty || actualPositionDirty) {
                CalculateActualPosition(renderer);
            }
        } else {
            if (billboardTarget.lock()->GetTransform().GetIsDirty() || true)
            {
                FollowTarget(renderer);
            }
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

    bool UIComponent::IsBillboard1() const {
        return isBillboard;
    }

    void UIComponent::SetIsBillboard(bool isBillboard) {
        UIComponent::isBillboard = isBillboard;
        actualPositionDirty = true;
    }

    const std::weak_ptr<Entity>& UIComponent::GetBillboardTarget() const {
        return billboardTarget;
    }

    void UIComponent::SetBillboardTarget(const std::weak_ptr<Entity>& billboardTarget) {
        UIComponent::billboardTarget = billboardTarget;
        isBillboard = true;
        actualPositionDirty = true;
    }


}