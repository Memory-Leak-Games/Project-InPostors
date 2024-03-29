#pragma once

#include "Gameplay/Component.h"
#include "SceneGraph/SceneGraphClient.h"
#include "UI/UIRenderable.h"

#define MLG_ANCHOR_CENTER glm::vec2{0.5, 0.5}
#define MLG_ANCHOR_LEFT glm::vec2{0.0, 0.5}
#define MLG_ANCHOR_RIGHT glm::vec2{1.0, 0.5}

#define MLG_POS_CENTER glm::vec2{1280.f * 0.5f, 720.f * 0.5f}

#define MLG_FULL_SIZE glm::vec2{1280.f, 720.f}

namespace mlg {
    /*
     * Base class of every ui component in engine, ex. Label
     */
    class UIComponent : public Component, public UIRenderable {
    private:
        glm::vec2 relativePosition = {0.0f, 0.0f};
        glm::vec2 position = {0.0f, 0.0f};

    protected:
        // Used for ui scaling
        glm::vec2 anchor{0.0f, 0.0f};
        glm::vec2 actualPosition = position;
//        bool actualPositionDirty = true;

        bool isBillboard = false;
        std::weak_ptr<ISceneGraphClient> billboardTarget;

        float padding = 0.f;

        bool visible = true;

        // If true, component will register itself in UIRenderer on Start()
        bool autoRegister = true;


    protected:
        virtual void CalculateActualPosition(const UIRenderer* renderer, const glm::vec2& position);
        void FollowTarget(const struct UIRenderer* renderer);


    public:
        std::weak_ptr<UIRenderable> thisAsRenderable;
        bool actualPositionDirty = true;
        UIComponent(std::weak_ptr<Entity> owner, std::string name);

        void Start() override;
        void Draw(const class UIRenderer* renderer) override;

        [[nodiscard]] const glm::vec2& GetRelativePosition() const;
        [[nodiscard]] const glm::vec2& GetAnchor() const;
        [[nodiscard]] virtual glm::vec2 GetSize() const = 0;
        [[nodiscard]] bool IsBillboard() const;
        [[nodiscard]] const std::weak_ptr<ISceneGraphClient>& GetBillboardTarget() const;
        [[nodiscard]] bool IsVisible() const;

        virtual void SetRelativePosition(const glm::vec2& position);
        virtual void SetAnchor(const glm::vec2& anchor);
        void SetIsBillboard(bool isBillboard);
        void SetBillboardTarget(const std::weak_ptr<ISceneGraphClient>& billboardTarget);
        virtual void SetVisible(bool visible);

        void SetPadding(float padding);
        float GetPadding() const;

        void SetAutoRegister(bool autoRegister);

        // Do not use this method. It is only for Containers
        virtual void SetPosition(const glm::vec2& position);
        glm::vec2 GetPosition() const;
    };
}// namespace mlg
