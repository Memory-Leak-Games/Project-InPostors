#pragma once

#include <utility>

#include "Gameplay/Component.h"
#include "UI/UIRenderable.h"

namespace mlg {
    /*
     * Base class of every ui component in engine, ex. Label
     */
    class UIComponent : public Component, public UIRenderable {
    private:
        // During rendering we are using actualPosition instead
        glm::vec2 position = {0.0f, 0.0f};

    protected:
        // Used for ui scaling
        glm::vec2 anchor{0.0f, 0.0f};
        glm::vec2 actualPosition = position;
        bool actualPositionDirty = true;

        bool isBillboard = false;
        std::weak_ptr<class Entity> billboardTarget;

        bool visible = true;


    protected:
        void CalculateActualPosition(const struct UIRenderer* renderer);
        void FollowTarget(const struct UIRenderer* renderer);

    public:
        std::weak_ptr<UIRenderable> thisAsRenderable;

        UIComponent(std::weak_ptr<Entity> owner, std::string name);

        void Start() override;
        void Draw(const class UIRenderer* renderer) override;

        [[nodiscard]] const glm::vec2& GetPosition() const;
        [[nodiscard]] const glm::vec2& GetAnchor() const;
        [[nodiscard]] bool IsBillboard1() const;
        [[nodiscard]] const std::weak_ptr<struct Entity>& GetBillboardTarget() const;
        [[nodiscard]] bool IsVisible() const;

        void SetPosition(const glm::vec2& position);
        void SetAnchor(const glm::vec2& anchor);
        void SetIsBillboard(bool isBillboard);
        void SetBillboardTarget(const std::weak_ptr<struct Entity>& billboardTarget);
        void SetVisible(bool visible);
    };
}// namespace mlg
