#include "ai/SteeringBehaviors.h"

SteeringBehaviors::SteeringBehaviors() : flags(0), deceleration(normal), summingMethod(prioritized) {
    cohesionWeight = .5f;
    separationWeight = .5f;
    alignmentWeight = .5f;
    seekWeight = .5f;
    arriveWeight = .5f;
    followPathWeight = .5f;

    //TODO: Set Path here
}

SteeringBehaviors::~SteeringBehaviors() {
    //TODO: Delete Path
}

bool SteeringBehaviors::AccumulateForce(glm::vec2& total, glm::vec2 forceToAdd) {
    float magnitudeThusFar = total.length();

//    float magnitudeRemaining =
}

glm::vec2 SteeringBehaviors::Calculate(float viewDistance) {
    steeringForce = { 0, 0 };

    if (BehaviorTypeOn(separation) || BehaviorTypeOn(alignment) || BehaviorTypeOn(cohesion)) {
        //TODO: Tag cars within view range
    }

    switch (summingMethod) {
        case weightedAverage:
            steeringForce = CalculateWeightedSum();
            break;
        case prioritized:
            steeringForce = CalculatePrioritized();
            break;
        case dithered:
            //TODO: Implement dithering in the future
            steeringForce = { 0, 0 };
            break;
        default:
            steeringForce = { 0, 0 };
    }

    return steeringForce;
}

glm::vec2 SteeringBehaviors::CalculatePrioritized() {
    glm::vec2 force;

    if (BehaviorTypeOn(separation)) {
        //TODO: Pass all cars here
//        force = Separation() * separationWeight;
    }
}

glm::vec2 SteeringBehaviors::CalculateWeightedSum() {
    glm::vec2 force;

    return force;
}
