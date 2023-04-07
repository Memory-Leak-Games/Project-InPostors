#pragma once

namespace mlg {

    namespace ColliderShape {
        class Rectangle;

        class Circle;
    }

    class CollisionDetection {
    public:
        static bool CircleCircleCollision(const ColliderShape::Circle* circleOne,
                                          const ColliderShape::Circle* circleTwo);

        static glm::vec2 CircleCircleSeparation(const ColliderShape::Circle* circleOne,
                                                const ColliderShape::Circle* circleTwo);

        static bool RectangleRectangleCollision(const ColliderShape::Rectangle* rectangleOne,
                                                const ColliderShape::Rectangle* rectangleTwo);

        static glm::vec2 SquareSquareSeparation(const ColliderShape::Rectangle* rectangleOne,
                                                const ColliderShape::Rectangle* rectangleTwo);

        static bool RectangleCircleCollision(const ColliderShape::Rectangle* rectangle,
                                             const ColliderShape::Circle* circle);

        static glm::vec2 SquareCircleSeparation(const ColliderShape::Rectangle* rectangle,
                                                const ColliderShape::Circle* circle);

        static glm::vec2 CalculateNearestPoint(const glm::vec2& position,
                                               const ColliderShape::Rectangle* rectangle);
    };


} // mlg
