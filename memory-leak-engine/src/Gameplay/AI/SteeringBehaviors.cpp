#include "Gameplay/AI/SteeringBehaviors.h"

namespace mlg {
    SteeringBehaviors::SteeringBehaviors()
        : flags(0),
        deceleration(normal) {
    }

    SteeringBehaviors::~SteeringBehaviors() {
    }

    glm::vec2 SteeringBehaviors::Calculate() {
        return glm::vec2();
    }
}// namespace mlg