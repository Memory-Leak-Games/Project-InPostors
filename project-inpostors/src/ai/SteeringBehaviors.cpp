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
    }
}

glm::vec2 SteeringBehaviors::CalculateWeightedSum() {
    glm::vec2 force;

    return force;
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
