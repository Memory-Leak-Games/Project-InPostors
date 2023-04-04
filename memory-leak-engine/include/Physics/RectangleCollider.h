#pragma once

#include "Collider.h"

namespace mlg {

    class RectangleCollider : public Collider {
    private:
        glm::vec2 size;

    public:
        explicit RectangleCollider(const glm::vec2 &size);

        ColliderType GetColliderType() override;
        float GetRadius() override;

        const glm::vec2 &GetSize() const;

        float GetTop();
        float GetBottom();
        float GetLeft();
        float GetRight();

    };

} // mlg
