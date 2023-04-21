#pragma once

namespace mlg {

    class Math {
    public:
        static constexpr float Cross2D(glm::vec2 v1, glm::vec2 v2) {
            return (v1.x * v2.y) - (v1.y * v2.x);
        }

        static constexpr float Sign(float x) {
            return x != 0.f ? x / std::abs(x) : 0.f;
        }

        template<typename T>
        static constexpr T SafeNormal(const T& x) {
            return glm::length(x) == 0.f ? T{0.f} : glm::normalize(x);
        }

        template<typename T>
        static constexpr T ClampVectorToNormal(T x) {
            return glm::length(x) > 1.f ? SafeNormal(x) : x;
        }

        template<typename T>
        static constexpr T Clamp(const T& x, const T& min, const T& max) {
            return std::clamp(x, min, max);
        }

        template<typename T>
        static constexpr T Sat(const T& x) {
            return Clamp(x, (T) 0, (T) 1);
        }

        template<typename T>
        static constexpr T Lerp(const T& a, const T& b, const T& t) {
            return a + (b - a) * t;
        }

        template<typename T>
        static constexpr T VectorProjection(const T& a, const T& b) {
            if (glm::length(b) == 0.)
                return T{0};

            return (glm::dot(a, b) / glm::dot(b, b)) * b;
        }
    };

} // mlg
