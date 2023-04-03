#pragma once

#include "Collider.h"

namespace mlg {

    class SquareCollider: Collider {
    private:
        glm::vec2 size;

    public:
        explicit SquareCollider(const glm::vec2 &size);

        ColliderType GetColliderType() override;
        float GetRadius() override;

        const glm::vec2 &GetSize() const;
    };

} // mlg
