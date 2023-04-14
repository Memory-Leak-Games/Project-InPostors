#include "Physics/CollisionDetection.h"

#include "Core/Math.h"

#include "Physics/Colliders/ColliderShapes.h"

namespace mlg {

    bool CollisionDetection::CircleCircleCollision(const ColliderShape::Circle* circleOne,
                                                   const ColliderShape::Circle* circleTwo) {
        const float radiusSum = circleOne->radius + circleTwo->radius;
        bool test = glm::length2(circleOne->position - circleTwo->position) <= radiusSum * radiusSum;
        return glm::length2(circleOne->position - circleTwo->position) <= radiusSum * radiusSum;
    }

    glm::vec2 CollisionDetection::CircleCircleSeparation(const ColliderShape::Circle* circleOne,
                                                         const ColliderShape::Circle* circleTwo) {
        const glm::vec2 oneToTwoVector = circleTwo->position - circleOne->position;
        const float separationLength = circleOne->radius + circleTwo->radius - glm::length(oneToTwoVector);

        return Math::SafeNormal(oneToTwoVector) * separationLength;
    }

    bool CollisionDetection::RectangleRectangleCollision(const ColliderShape::Rectangle* rectangleOne,
                                                         const ColliderShape::Rectangle* rectangleTwo) {
        return rectangleOne->GetRight() > rectangleTwo->GetLeft()
               && rectangleOne->GetLeft() < rectangleTwo->GetRight()
               && rectangleOne->GetTop() > rectangleTwo->GetBottom()
               && rectangleOne->GetBottom() < rectangleTwo->GetTop();
    }

    glm::vec2 CollisionDetection::RectangleRectangleSeparation(const ColliderShape::Rectangle* rectangleOne,
                                                               const ColliderShape::Rectangle* rectangleTwo) {
        float leftSeparation = rectangleOne->GetRight() - rectangleTwo->GetLeft();
        float rightSeparation = rectangleTwo->GetRight() - rectangleOne->GetLeft();
        float topSeparation = rectangleOne->GetTop() - rectangleTwo->GetBottom();
        float bottomSeparation = rectangleTwo->GetTop() - rectangleOne->GetBottom();

        glm::vec2 finalSeparation;
        finalSeparation.x = leftSeparation < rightSeparation ? -leftSeparation : rightSeparation;
        finalSeparation.y = bottomSeparation < topSeparation ? bottomSeparation : -topSeparation;

        if (std::abs(finalSeparation.x) < std::abs(finalSeparation.y))
            finalSeparation.y = 0.f;
        else
            finalSeparation.x = 0.f;

        return -finalSeparation;
    }

    bool CollisionDetection::RectangleCircleCollision(const ColliderShape::Rectangle* rectangle,
                                                      const ColliderShape::Circle* circle) {
        glm::vec2 nearestPoint = CalculateNearestPointOnRect(circle->position, rectangle);
        return glm::length(circle->position - nearestPoint) <= circle->radius;
    }

    glm::vec2 CollisionDetection::CalculateNearestPointOnRect(const glm::vec2& position,
                                                              const ColliderShape::Rectangle* rectangle) {
        glm::vec2 result;
        result.x = glm::clamp(position.x, rectangle->GetLeft(), rectangle->GetRight());
        result.y = glm::clamp(position.y, rectangle->GetBottom(), rectangle->GetTop());

        return result;
    }

    glm::vec2 CollisionDetection::RectangleCircleSeparation(const ColliderShape::Rectangle* rectangle,
                                                            const ColliderShape::Circle* circle) {
        glm::vec2 nearestPoint = CalculateNearestPointOnRect(circle->position, rectangle);

        auto rectangleAsCircle = std::make_unique<ColliderShape::Circle>(nearestPoint, glm::vec2{0.f}, 0.f);
        return CircleCircleSeparation(rectangleAsCircle.get(), circle);
    }

    glm::vec2 CollisionDetection::FindCollisionPointForCircle(const mlg::ColliderShape::Circle* circle,
                                                              const glm::vec2& anotherPosition) {
        glm::vec2 oneToTwoDirection = -Math::SafeNormal(circle->position - anotherPosition);
        return oneToTwoDirection * circle->radius + circle->position;
    }

    glm::vec2 CollisionDetection::FindCollisionPointForRect(const ColliderShape::Rectangle* rectangle,
                                                            const glm::vec2& anotherPosition) {
        glm::vec2 oneToTwoDirection = -Math::SafeNormal(rectangle->position - anotherPosition);
        return oneToTwoDirection * rectangle->GetRadius() + rectangle->position;
    }


} // mlg