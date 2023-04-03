#include "Physics/SquareCollider.h"

namespace mlg {
    SquareCollider::SquareCollider(const glm::vec2 &size) : size(size) {}

    const glm::vec2 &SquareCollider::GetSize() const {
        return size;
    }

    ColliderType SquareCollider::GetColliderType() {
        return ColliderType::Square;
    }

    float SquareCollider::GetRadius() {
        return std::sqrt(size.x + size.y) / 2.f;
    }
} // mlg