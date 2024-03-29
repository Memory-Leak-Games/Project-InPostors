#include "Physics/Colliders/Collider.h"

#include "Physics/CollisionDetection.h"
#include "Physics/Colliders/ColliderShapes.h"

namespace mlg {
    Collider::Collider(Rigidbody* owner, std::unique_ptr<ColliderShape::Shape> shape)
            : owner(owner), shape(std::move(shape)) {}

    Collider::Collider(Rigidbody* owner, bool isTrigger, std::unique_ptr<ColliderShape::Shape> shape)
        : Collider(owner, std::move(shape)) {
        this->isTrigger = isTrigger;
    }

    Collider::~Collider() = default;

    const Rigidbody* Collider::GetOwner() const {
        return owner;
    }

    bool Collider::DetectCollision(Collider* anotherCollider) {
        if (shape->GetType() == ColliderShape::ColliderShapeType::Circle) {
            return DetectCollisionAsCircle(anotherCollider);
        } else {
            return DetectCollisionAsRectangle(anotherCollider);
        }
    }

    bool Collider::DetectCollisionBoundingCircle(Collider* anotherCollider) const {
        const float radius = GetRadius();
        const float anotherRadius = anotherCollider->GetRadius();

        const float distance = glm::distance(this->GetPosition(), anotherCollider->GetPosition());

        return radius + anotherRadius >= distance;
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
        if (anotherCollider->shape->GetType() == ColliderShape::ColliderShapeType::Rectangle) {
            return CollisionDetection::RectangleRectangleCollision((ColliderShape::Rectangle*) shape.get(),
                                                                   (ColliderShape::Rectangle*) anotherCollider->shape.get());
        } else {
            return CollisionDetection::RectangleCircleCollision((ColliderShape::Rectangle*) shape.get(),
                                                                (ColliderShape::Circle*) anotherCollider->shape.get());
        }
    }

    void Collider::Separate(Collider* anotherCollider, glm::vec2 separationVector) {
        if (anotherCollider->owner->isKinematic) {
            owner->position -= separationVector;
        } else {
            owner->position -= separationVector * 0.5f;
            anotherCollider->owner->position += separationVector * 0.5f;
        }
    }

    glm::vec2 Collider::CalculateSeparation(Collider* anotherCollider) {
        if (shape->GetType() == ColliderShape::ColliderShapeType::Circle) {
            return CalculateSeparationAsCircle(anotherCollider);
        } else {
            return CalculateSeparationAsRectangle(anotherCollider);
        }
    }

    glm::vec2 Collider::CalculateSeparationAsCircle(Collider* anotherCollider) {
        if (anotherCollider->shape->GetType() == ColliderShape::ColliderShapeType::Circle) {
            return CollisionDetection::CircleCircleSeparation((ColliderShape::Circle*) shape.get(),
                                                              (ColliderShape::Circle*) anotherCollider->shape.get());
        } else {
            return -CollisionDetection::RectangleCircleSeparation((ColliderShape::Rectangle*) anotherCollider->shape.get(),
                                                                 (ColliderShape::Circle*) shape.get());
        }
    }

    glm::vec2 Collider::CalculateSeparationAsRectangle(Collider* anotherCollider) {
        if (anotherCollider->shape->GetType() == ColliderShape::ColliderShapeType::Rectangle) {
            return CollisionDetection::RectangleRectangleSeparation((ColliderShape::Rectangle*) shape.get(),
                                                                    (ColliderShape::Rectangle*) anotherCollider->shape.get());
        } else {
            return CollisionDetection::RectangleCircleSeparation((ColliderShape::Rectangle*) shape.get(),
                                                                 (ColliderShape::Circle*) anotherCollider->shape.get());
        }
    }

    glm::vec2 Collider::FindCollisionPoint(const glm::vec2& anotherPosition) {
        if (shape->GetType() == ColliderShape::ColliderShapeType::Circle) {
            return CollisionDetection::FindCollisionPointForCircle((ColliderShape::Circle*) this->shape.get(),
                                                                   anotherPosition);
        } else {
            return CollisionDetection::FindCollisionPointForRect((ColliderShape::Rectangle*) this->shape.get(),
                                                                 anotherPosition);
        }
    }

    glm::vec2 Collider::GetPosition() const {
        return shape->position;
    }

    ColliderShape::AABB Collider::GetAABB() const {
        return shape->GetAABB();
    }

    float Collider::GetRadius() const {
        return shape->GetRadius();
    }

    bool Collider::GetIsTrigger() const {
        return isTrigger;
    }

    const std::string& Collider::GetTag() const {
        return tag;
    }

    void Collider::SetTag(const std::string& tag) {
        Collider::tag = tag;
    }

}
