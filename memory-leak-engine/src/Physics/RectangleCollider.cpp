#include "Physics/RectangleCollider.h"

#include "Physics/Rigidbody.h"

namespace mlg {
    RectangleCollider::RectangleCollider(const glm::vec2 &size) : size(size) {}

    const glm::vec2 &RectangleCollider::GetSize() const {
        return size;
    }

    ColliderType RectangleCollider::GetColliderType() {
        return ColliderType::Square;
    }

    float RectangleCollider::GetRadius() {
        return std::sqrt(size.x + size.y) / 2.f;
    }

    float RectangleCollider::GetTop() {
        return owner->GetPosition().y + size.y / 2;
    }

    float RectangleCollider::GetBottom() {
        return owner->GetPosition().y - size.y / 2;
    }

    float RectangleCollider::GetLeft() {
        return owner->GetPosition().x - size.x / 2;
    }

    float RectangleCollider::GetRight() {
        return owner->GetPosition().x + size.x / 2;
    }
} // mlg