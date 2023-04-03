#include "Physics/CollisionResponses.h"

#include "Core/Math.h"

#include "Physics/Rigidbody.h"
#include "Physics/CircleCollider.h"
#include "Physics/RectangleCollider.h"

namespace mlg {
    bool CollisionResponses::CircleCircleCollision(CircleCollider *circleOne, CircleCollider *circleTwo) {
        glm::vec2 positionOne = circleOne->owner->position;
        glm::vec2 positionTwo = circleTwo->owner->position;

        float radiusOne = circleOne->GetRadius();
        float radiusTwo = circleTwo->GetRadius();

        float radiusSum = radiusOne + radiusTwo;

        return glm::length2(positionOne - positionTwo) <= radiusSum * radiusSum;
    }

    glm::vec2 CollisionResponses::CircleCircleSeparationVector(CircleCollider *circleOne,
                                                               CircleCollider *circleTwo) {
        glm::vec2 positionOne = circleOne->owner->position;
        glm::vec2 positionTwo = circleTwo->owner->position;

        float radiusOne = circleOne->GetRadius();
        float radiusTwo = circleTwo->GetRadius();

        glm::vec2 oneToTwo = positionTwo - positionOne;
        float separationLength = radiusOne + radiusTwo - glm::length(oneToTwo);

        return Math::SafeNormal(oneToTwo) * separationLength;
    }

    bool CollisionResponses::SquareSquareCollision(RectangleCollider *rectangleOne, RectangleCollider *rectangleTwo) {
        return rectangleOne->GetRight() > rectangleTwo->GetLeft()
               && rectangleOne->GetLeft() < rectangleTwo->GetRight()
               && rectangleOne->GetTop() > rectangleTwo->GetBottom()
               && rectangleOne->GetBottom() < rectangleTwo->GetTop();
    }

    glm::vec2 CollisionResponses::SquareSquareSeparationVector(RectangleCollider *rectangleOne,
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

    bool CollisionResponses::SquareCircleCollision(RectangleCollider *rectangle, CircleCollider *circle) {
        glm::vec2 rectanglePosition = rectangle->owner->position;

        glm::vec2 nearestPoint = CalculateNearestPoint(rectangle->o);
        return glm::length(thisPosition - nearestPoint) <= thisCollisionShape->radius;
    }

    glm::vec2 CollisionResponses::CalculateNearestPoint(const glm::vec2 &position, RectangleCollider *rectangle) {
        glm::vec2 result;
        result.x = glm::clamp(position.x, rectangle->GetLeft(), rectangle->GetRight());
        result.y = glm::clamp(position.y, rectangle->GetBottom(), rectangle->GetTop());

        return result;
    }

} // mlg