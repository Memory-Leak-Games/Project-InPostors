#pragma once

#include <memory>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "Gizmo.h"

namespace mlg {
    class Arrow: public Gizmo
    {
    public:
        static void Draw(glm::vec3 Start, glm::vec3 End, glm::vec4 Color);
    };
}
