#pragma once

#include "Levels/LevelGenerator.h"

struct AnimatedEntityData {
    int id;
    mlg::MapObject* mapObject;
    glm::ivec2 position;
    glm::vec3 objectPosition;
    float rotation;
};

class FerrisWheelAnim {

public:
    FerrisWheelAnim();

    void Spawn(AnimatedEntityData* animData);

    ~FerrisWheelAnim() = default;
};
