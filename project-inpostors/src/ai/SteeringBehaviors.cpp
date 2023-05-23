#include <utility>

#include <nlohmann/json.hpp>
#include <fstream>

#include "ai/SteeringBehaviors.h"
#include "ai/TrafficCar.h"
#include "ai/AIComponent.h"

using json = nlohmann::json;

SteeringBehaviors::SteeringBehaviors(AIComponent* agent, const std::string& configPath)
    : aiComponent(agent), flags(0), deceleration(normal), summingMethod(prioritized) {
    LoadParameters(configPath);

    //TODO: Set Path here
}

SteeringBehaviors::~SteeringBehaviors() {
    //TODO: Delete Path
}

bool SteeringBehaviors::AccumulateForce(glm::vec2& total, glm::vec2 forceToAdd) {
    float magnitudeThusFar = total.length();

    float magnitudeRemaining = aiComponent->GetMaxForce() - magnitudeThusFar;

    if (magnitudeRemaining <= 0.0)
        return false;

    float magnitudeToAdd = forceToAdd.length();

    if (magnitudeToAdd < magnitudeRemaining)
        total += forceToAdd;
    else
        total += glm::normalize(forceToAdd) * magnitudeRemaining;

    return true;
}

glm::vec2 SteeringBehaviors::Calculate() {
    steeringForce = { 0, 0 };

    if (BehaviorTypeOn(separation) || BehaviorTypeOn(alignment)) {
        //TODO: Tag cars within view range
    }

    switch (summingMethod) {
        case weightedAverage:
            //TODO: Implement weighted average in the future
            steeringForce = { 0, 0 };
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

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(alignment)) {
        //TODO: Pass all cars here
        //        force = Alignment() * alignmentWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(seek)) {
        force = Seek({20, 2}) * seekWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(arrive)) {
        force = Arrive({10, 10}, deceleration) * arriveWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(followPath)) {
//        force = FollowPath() * followPathWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    return {0, 0};
}

glm::vec2 SteeringBehaviors::Seek(glm::vec2 TargetPos) {
    glm::vec2 desiredVelocity = glm::normalize(TargetPos - aiComponent->GetPosition())
                                * aiComponent->GetPosition();

    return (desiredVelocity - aiComponent->GetLinearVelocity());
}

glm::vec2 SteeringBehaviors::Arrive(glm::vec2 TargetPos, Deceleration deceleration) {
    glm::vec2 toTarget = TargetPos - aiComponent->GetPosition();

    float distance = toTarget.length();

    if (distance > 0) {
        const float decelerationTweaker = 0.3;

        float speed = distance / ((float)deceleration * decelerationTweaker);
        speed = fmin(speed, aiComponent->GetMaxSpeed());

        glm::vec2 desiredVelocity = toTarget * speed / distance;
        return (desiredVelocity - aiComponent->GetLinearVelocity());
    }

    return glm::vec2(0, 0);
}

void SteeringBehaviors::LoadParameters(const std::string& path) {
    std::ifstream configFile{path};
    json configJson = json::parse(configFile);

    auto parameters = configJson["parameters"];

    separationWeight = parameters["separationWeight"];
    alignmentWeight = parameters["alignmentWeight"];
    seekWeight = parameters["seekWeight"];
    arriveWeight = parameters["arriveWeight"];
    followPathWeight = parameters["followPathWeight"];
}
