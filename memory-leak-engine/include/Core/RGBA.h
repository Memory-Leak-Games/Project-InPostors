#pragma once

#include "glm/vec4.hpp"

namespace mlg {

    typedef glm::vec<4, float> RGBA;
    typedef glm::vec<3, float> RGB;

    class Colors {
    public:
        Colors() = delete;
        ~Colors() = delete;

        static constexpr RGBA white = glm::vec4{1.f, 1.f, 1.f, 1.f};
        static constexpr RGBA black = glm::vec4{0.f, 0.f, 0.f, 1.f};
        static constexpr RGBA red = glm::vec4{1.f, 0.f, 0.f, 1.f};
        static constexpr RGBA green = glm::vec4{0.f, 1.f, 0.f, 1.f};
        static constexpr RGBA blue = glm::vec4{0.f, 1.f, 0.f, 1.f};
        static constexpr RGBA yellow = glm::vec4{1.f, 1.f, 0.f, 1.f};
        static constexpr RGBA magenta = glm::vec4{1.f, 0.f, 1.f, 1.f};
        static constexpr RGBA cyan = glm::vec4{0.f, 1.f, 1.f, 1.f};
    };

} // mlg
