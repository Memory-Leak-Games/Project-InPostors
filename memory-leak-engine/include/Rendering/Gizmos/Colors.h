#pragma once

#include "glm/glm.hpp"

/*
 *  Is a class in this case an overkill? Yes.
 *  I've tried to make Colors a namespace with variables, to reduce boilerplate to minimum.
 *  Unfortunately, linker didn't like that :(
 */
namespace mlg {
    class Colors {
    public:
        static glm::vec4 Red;
        static glm::vec4 Green;
        static glm::vec4 Blue;

        static glm::vec4 Yellow;
        static glm::vec4 Cyan;
        static glm::vec4 Purple;

        static glm::vec4 White;
        static glm::vec4 Black;
        static glm::vec4 Transparent;
    };
}
