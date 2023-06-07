#pragma once

#include <utility>

#include "Gameplay/Component.h"
#include "UI/UIRenderable.h"

#define MLG_ANCHOR_CENTER glm::vec2{0.5, 0.5}
#define MLG_POS_CENTER glm::vec2{1280.f * 0.5f, 720.f * 0.5f}

namespace mlg {
    /*
     * Base class of every ui component in engine, ex. Label
     */
    class UIComponent : public Component, public UIRenderable {
    private:
        glm::vec2 position = {0.0f, 0.0f};

    protected:
        // Used for ui scaling
        glm::vec2 anchor{0.0f, 0.0f};
        glm::vec2 actualPosition = position;
        bool actualPositionDirty = true;

        bool isBillboard = false;
        std::weak_ptr<class Entity> billboardTarget;

        bool visible = true;

        // If true, component will register itself in UIRenderer on Start()
        bool autoRegister = true;


    protected:
        virtual void CalculateActualPosition(const UIRenderer* renderer, const glm::vec2& position);
        void FollowTarget(const struct UIRenderer* renderer);

    public:
        std::weak_ptr<UIRenderable> thisAsRenderable;

        UIComponent(std::weak_ptr<Entity> owner, std::string name);

        void Start() override;
        void Draw(const class UIRenderer* renderer) override;

        [[nodiscard]] const glm::vec2& GetPosition() const;
        [[nodiscard]] const glm::vec2& GetAnchor() const;
        [[nodiscard]] bool IsBillboard() const;
        [[nodiscard]] const std::weak_ptr<struct Entity>& GetBillboardTarget() const;
        [[nodiscard]] bool IsVisible() const;

        virtual void SetPosition(const glm::vec2& position);
        virtual void SetAnchor(const glm::vec2& anchor);
        void SetIsBillboard(bool isBillboard);
        void SetBillboardTarget(const std::weak_ptr<struct Entity>& billboardTarget);
        virtual void SetVisible(bool visible);

        void SetAutoRegister(bool autoRegister);
    };
}// namespace mlg
