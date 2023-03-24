#pragma once

#include <memory>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "OldGizmo.h"

namespace mlg {
    class Arrow: public OldGizmo
    {
    public:
        static void Draw(glm::vec3 Start, glm::vec3 End, glm::vec4 Color);
    };
}
