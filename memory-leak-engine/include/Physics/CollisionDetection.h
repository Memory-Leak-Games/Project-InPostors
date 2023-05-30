#pragma once

#include <glm/fwd.hpp>

namespace mlg {

    namespace ColliderShape {
        class Rectangle;

        class Circle;
    }// namespace ColliderShape

    class CollisionDetection {
    public:
        static bool CircleCircleCollision(
                const ColliderShape::Circle* circleOne,
                const ColliderShape::Circle* circleTwo);

        static glm::vec2 CircleCircleSeparation(
                const ColliderShape::Circle* circleOne,
                const ColliderShape::Circle* circleTwo);

        static bool RectangleRectangleCollision(
                const ColliderShape::Rectangle* rectangleOne,
                const ColliderShape::Rectangle* rectangleTwo);

        static glm::vec2 RectangleRectangleSeparation(
                const ColliderShape::Rectangle* rectangleOne,
                const ColliderShape::Rectangle* rectangleTwo);

        static bool RectangleCircleCollision(
                const ColliderShape::Rectangle* rectangle,
                const ColliderShape::Circle* circle);

        static glm::vec2 RectangleCircleSeparation(
                const ColliderShape::Rectangle* rectangle,
                const ColliderShape::Circle* circle);

        static glm::vec2 CalculateNearestPointOnRect(
                const glm::vec2& position,
                const ColliderShape::Rectangle* rectangle);

        static glm::vec2 FindCollisionPointForCircle(
                const mlg::ColliderShape::Circle* circle,
                const glm::vec2& anotherPosition);
        static glm::vec2 FindCollisionPointForRect(
                const ColliderShape::Rectangle* rectangle,
                const glm::vec2& anotherPosition);
    };


}// namespace mlg
