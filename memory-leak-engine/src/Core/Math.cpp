#include "Core/Math.h"

namespace mlg {
    constexpr float Math::Cross2D(glm::vec2 v1, glm::vec2 v2) {
        return (v1.x * v2.y) - (v1.y * v2.x);
    }
} // mlg