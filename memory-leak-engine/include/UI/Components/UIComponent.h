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
        glm::vec2 anchor {0.0f, 0.0f};
        glm::vec2 actualPosition = position;
        bool actualPositionDirty = true;
        void CalculateActualPosition(const struct UIRenderer* renderer);

    public:
        UIComponent(std::weak_ptr<Entity> owner, std::string name);

        void Draw(const class UIRenderer *renderer) override;

        [[nodiscard]] const glm::vec2& GetPosition() const;
        [[nodiscard]] const glm::vec2& GetAnchor() const;

        void SetPosition(const glm::vec2& position);
        void SetAnchor(const glm::vec2& anchor);
    };
}