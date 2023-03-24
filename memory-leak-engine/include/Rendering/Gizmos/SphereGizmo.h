#pragma once


#include "OldGizmo.h"
#include "glm/glm.hpp"

namespace mlg {
    class SphereGizmo: public OldGizmo
    {
    public:
        static void Draw(glm::vec3 Position, float Radius, uint16_t LOD, glm::vec4 Color);
    };
}
