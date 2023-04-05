#include "Physics/CollisionDetection.h"

#include "Core/Math.h"

#include "Physics/Rigidbody.h"
#include "include/Physics/Colliders/CircleCollider.h"
#include "include/Physics/Colliders/RectangleCollider.h"

namespace mlg {
    bool CollisionDetection::CircleCircleCollision(CircleCollider *circleOne,
                                                   CircleCollider *circleTwo) {
        glm::vec2 positionOne = circleOne->owner->position;
        glm::vec2 positionTwo = circleTwo->owner->position;

        float radiusOne = circleOne->GetRadius();
        float radiusTwo = circleTwo->GetRadius();

        float radiusSum = radiusOne + radiusTwo;

        return glm::length2(positionOne - positionTwo) <= radiusSum * radiusSum;
    }

    glm::vec2 CollisionDetection::CircleCircleSeparation(CircleCollider *circleOne,
                                                         CircleCollider *circleTwo) {
        glm::vec2 positionOne = circleOne->owner->position;
        glm::vec2 positionTwo = circleTwo->owner->position;

        float radiusOne = circleOne->GetRadius();
        float radiusTwo = circleTwo->GetRadius();

        glm::vec2 oneToTwo = positionTwo - positionOne;
        float separationLength = radiusOne + radiusTwo - glm::length(oneToTwo);

        return Math::SafeNormal(oneToTwo) * separationLength;
    }

    bool CollisionDetection::SquareSquareCollision(RectangleCollider *rectangleOne,
                                                   RectangleCollider *rectangleTwo) {
        return rectangleOne->GetRight() > rectangleTwo->GetLeft()
               && rectangleOne->GetLeft() < rectangleTwo->GetRight()
               && rectangleOne->GetTop() > rectangleTwo->GetBottom()
               && rectangleOne->GetBottom() < rectangleTwo->GetTop();
    }

    glm::vec2 CollisionDetection::SquareSquareSeparation(RectangleCollider *rectangleOne,
                                                         RectangleCollider *rectangleTwo) {
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

        return finalSeparation;
    }

    bool CollisionDetection::SquareCircleCollision(RectangleCollider *rectangle, CircleCollider *circle) {
        glm::vec2 rectanglePosition = rectangle->owner->position;
        glm::vec2 circlePosition = circle->owner->position;

        glm::vec2 nearestPoint = CalculateNearestPoint(circlePosition, rectangle);
        return glm::length(circlePosition - nearestPoint) <= circle->GetRadius();
    }

    glm::vec2 CollisionDetection::CalculateNearestPoint(const glm::vec2 &position, RectangleCollider *rectangle) {
        glm::vec2 result;
        result.x = glm::clamp(position.x, rectangle->GetLeft(), rectangle->GetRight());
        result.y = glm::clamp(position.y, rectangle->GetBottom(), rectangle->GetTop());

        return result;
    }

    glm::vec2 CollisionDetection::SquareCircleSeparation(RectangleCollider *rectangle, CircleCollider *circle) {
        glm::vec2 rectanglePosition = rectangle->owner->position;
        glm::vec2 circlePosition = circle->owner->position;
        glm::vec2 nearestPoint = CalculateNearestPoint(circlePosition, rectangle);

        glm::vec2 circleToNearestPointVector = circlePosition - nearestPoint;
        glm::vec2 circleToNearestPointDirection = Math::SafeNormal(circleToNearestPointVector);

        float circleToNearestPointDistance = glm::length(circleToNearestPointDistance);
        float separationLength = circle->GetRadius() - circleToNearestPointDistance;

        return circleToNearestPointDirection * separationLength;
    }

} // mlg