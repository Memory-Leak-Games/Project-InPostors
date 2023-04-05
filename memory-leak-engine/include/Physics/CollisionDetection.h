
#pragma once

namespace mlg {

    class CollisionDetection {
    public:
        static bool CircleCircleCollision(class CircleCollider* circleOne, CircleCollider* circleTwo);
        static glm::vec2 CircleCircleSeparation(class CircleCollider *circleOne, CircleCollider *circleTwo);

        static bool SquareSquareCollision(class RectangleCollider* rectangleOne, RectangleCollider* rectangleTwo);
        static glm::vec2 SquareSquareSeparation(class RectangleCollider *rectangleOne, RectangleCollider *rectangleTwo);

        static bool SquareCircleCollision(RectangleCollider* rectangle, CircleCollider* circle);
        static glm::vec2 SquareCircleSeparation(RectangleCollider* rectangle, CircleCollider* circle);

        static glm::vec2 CalculateNearestPoint(const glm::vec2& position, RectangleCollider* rectangle);
    };



} // mlg
