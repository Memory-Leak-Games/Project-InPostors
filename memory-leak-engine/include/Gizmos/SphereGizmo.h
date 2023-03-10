#pragma once


#include "Gizmo.h"
#include "glm/glm.hpp"

class SphereGizmo: public Gizmo
{
public:
    static void Draw(glm::vec3 Position, float Radius, uint16_t LOD, glm::vec4 Color);
};
