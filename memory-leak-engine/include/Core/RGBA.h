#pragma once

#include "glm/vec4.hpp"

namespace mlg {

    class RGBA {
    public:
        RGBA() = delete;
        ~RGBA() = delete;

        static constexpr glm::vec4 white = glm::vec4{1.f, 1.f, 1.f, 1.f};
        static constexpr glm::vec4 black = glm::vec4{0.f, 0.f, 0.f, 1.f};
        static constexpr glm::vec4 red = glm::vec4{1.f, 0.f, 0.f, 1.f};
        static constexpr glm::vec4 green = glm::vec4{0.f, 1.f, 0.f, 1.f};
        static constexpr glm::vec4 blue = glm::vec4{0.f, 1.f, 0.f, 1.f};
        static constexpr glm::vec4 yellow = glm::vec4{1.f, 1.f, 0.f, 1.f};
        static constexpr glm::vec4 magenta = glm::vec4{1.f, 0.f, 1.f, 1.f};
        static constexpr glm::vec4 cyan = glm::vec4{0.f, 1.f, 1.f, 1.f};
    };

} // mlg
