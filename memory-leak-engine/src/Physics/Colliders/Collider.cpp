#include "Physics/Colliders/Collider.h"

#include "Physics/CollisionDetection.h"
#include "Physics/Colliders/ColliderShapes.h"

namespace mlg {
    Collider::Collider(Rigidbody* owner, std::unique_ptr<ColliderShape::Shape> shape)
    : owner(owner), shape(std::move(shape)) {}

    Collider::~Collider() = default;

    const Rigidbody* Collider::GetOwner() const {
        return owner;
    }

    bool Collider::DetectCollision(Collider* anotherCollider) {
        shape->position = owner->GetPosition();
        anotherCollider->shape->position = anotherCollider->owner->GetPosition();

        if (shape->GetType() == ColliderShape::ColliderShapeType::Circle) {
            return DetectCollisionAsCircle(anotherCollider);
        } else {
            return DetectCollisionAsRectangle(anotherCollider);
        }
    }

    bool Collider::DetectCollisionAsCircle(Collider* anotherCollider) {
        if (anotherCollider->shape->GetType() == ColliderShape::ColliderShapeType::Circle) {
            return CollisionDetection::CircleCircleCollision((ColliderShape::Circle*) shape.get(),
                                                             (ColliderShape::Circle*) anotherCollider->shape.get());
        } else {
            return CollisionDetection::RectangleCircleCollision(
                    (ColliderShape::Rectangle*) anotherCollider->shape.get(), (ColliderShape::Circle*) shape.get());
        }
    }

    bool Collider::DetectCollisionAsRectangle(Collider* anotherCollider) {
        if (anotherCollider->shape->GetType() == ColliderShape::ColliderShapeType::Circle) {
            return CollisionDetection::RectangleRectangleCollision((ColliderShape::Rectangle*) shape.get(),
                                                                   (ColliderShape::Rectangle*) anotherCollider->shape.get());
        } else {
            return CollisionDetection::RectangleCircleCollision((ColliderShape::Rectangle*) shape.get(), (ColliderShape::Circle*) anotherCollider->shape.get()); }
    }
}
