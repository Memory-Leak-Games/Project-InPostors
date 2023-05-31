#pragma once

#include "glm/vec2.hpp"
#include <glm/fwd.hpp>

namespace mlg::ColliderShape {

    enum class ColliderShapeType {
        Circle,
        Rectangle
    };

    struct AABB {
        glm::vec2 min;
        glm::vec2 max;
    };

    struct Shape {
        glm::vec2 position{0.f};
        glm::vec2 offset{0.f};

        Shape(const glm::vec2& position, const glm::vec2& offset) : position(position), offset(offset) {}

        virtual ~Shape() = default;

        [[nodiscard]] virtual AABB GetAABB() const = 0;
        [[nodiscard]] virtual float GetRadius() const = 0;
        [[nodiscard]] virtual ColliderShapeType GetType() const = 0;
    };

    struct Circle : public Shape {
        float radius = 0.f;

        Circle(const glm::vec2& position, const glm::vec2& offset, float radius)
            : Shape(position, offset), radius(radius) {}

        ~Circle() override = default;

        [[nodiscard]] AABB GetAABB() const override {
            return {
                    position - radius * glm::vec2(1.f),
                    position + radius * glm::vec2(1.f)};
        }

        [[nodiscard]] float GetRadius() const override {
            return radius;
        }

        [[nodiscard]] ColliderShapeType GetType() const override {
            return ColliderShapeType::Circle;
        }
    };

    struct Rectangle : public Shape {
        glm::vec2 size{0.f};
        const float boundingCircleRadius;

        Rectangle(const glm::vec2& position, const glm::vec2& offset, const glm::vec2& size)
            : Shape(position, offset), size(size), boundingCircleRadius(glm::length(size) * 0.5f) {}

        ~Rectangle() override = default;

        [[nodiscard]] AABB GetAABB() const override {
            return {
                    position - size * glm::vec2(1.f) * 0.5f,
                    position + size * glm::vec2(1.f) * 0.5f};
        }

        [[nodiscard]] float GetRadius() const override {
            return boundingCircleRadius;
        }

        [[nodiscard]] ColliderShapeType GetType() const override {
            return ColliderShapeType::Rectangle;
        }

        [[nodiscard]] float GetTop() const {
            return position.y + (size.y * 0.5f);
        }

        [[nodiscard]] float GetBottom() const {
            return position.y - (size.y * 0.5f);
        }

        [[nodiscard]] float GetLeft() const {
            return position.x - (size.x * 0.5f);
        }

        [[nodiscard]] float GetRight() const {
            return position.x + (size.x * 0.5f);
        }
    };

}// namespace mlg::ColliderShape