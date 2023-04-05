#include "include/Physics/Colliders/CircleCollider.h"

namespace mlg {
    CircleCollider::CircleCollider(float radius) : radius(radius) {}

    ColliderType CircleCollider::GetColliderType() {
        return ColliderType::Circle;
    }

    float CircleCollider::GetRadius() {
        return radius;
    }
}// namespace mlg