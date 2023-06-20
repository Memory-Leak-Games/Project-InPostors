#pragma once

#include "Gameplay/Entity.h"

struct AnimatedEntityData {
    int id;
    std::string modelPath;
    std::string materialPath;
    std::string colliderType;
    float worldRot;
    float scale;
    float colliderSize = 1.0f;
    float colliderOffset = 0.0f;
    bool hasCollision = false;

    glm::ivec2 position;
    float rotation;
};

class AnimatedEntity : public mlg::Entity {
};
