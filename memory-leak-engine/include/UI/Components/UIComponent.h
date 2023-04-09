#pragma once

#include <utility>

#include "Gameplay/Component.h"
#include "UI/Renderable2D.h"

namespace mlg {
    /*
     * Base class of every ui component in engine, ex. Label
     */
    class UIComponent : public Component, public Renderable2D {
    protected:
        glm::vec2 position = {0.0f, 0.0f};

    public:
        UIComponent(std::weak_ptr<Entity> owner, std::string name);

        [[nodiscard]] const glm::vec2& GetPosition() const;

        void SetPosition(const glm::vec2& position);
    };
}