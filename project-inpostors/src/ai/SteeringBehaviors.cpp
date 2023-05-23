#include <utility>

#include <nlohmann/json.hpp>
#include <fstream>

#include "ai/SteeringBehaviors.h"
#include "ai/TrafficCar.h"
#include "ai/AIComponent.h"

#include "Gameplay/EntityManager.h"
#include "Gameplay/Entity.h"

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
    std::vector<std::weak_ptr<TrafficCar>> cars = mlg::EntityManager::FindAllByType<TrafficCar>();

    if (BehaviorTypeOn(separation)) {
        force = Separation(cars) * separationWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(alignment)) {
        force = Alignment(cars) * alignmentWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(seek)) {
        force = Seek({10, -20}) * seekWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(arrive)) {
        force = Arrive({10, -20}, deceleration) * arriveWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(followPath)) {
//        force = FollowPath() * followPathWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    return steeringForce;
}

glm::vec2 SteeringBehaviors::Seek(glm::vec2 TargetPos) {
    glm::vec2 desiredVelocity = glm::normalize(TargetPos - aiComponent->GetPosition())
                                * aiComponent->GetMaxSpeed();

    glm::vec2 velocity2D;
    velocity2D.x = aiComponent->GetLocalVelocity().x;
    velocity2D.y = aiComponent->GetLocalVelocity().z;
    return (desiredVelocity - velocity2D);
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

glm::vec2 SteeringBehaviors::Separation(const std::vector<std::weak_ptr<TrafficCar>>& agents) {
    glm::vec2 steerForce;

    for (const auto & agent : agents) {
        if (agent != aiComponent->GetOwner()) {
            glm::vec2 toAgent = aiComponent->GetPosition() - agent.lock()->GetComponentByClass<AIComponent>().lock()->GetPosition();

            glm::vec2 normToAgent = glm::normalize(toAgent);
            normToAgent.x /= toAgent.length();
            normToAgent.y /= toAgent.length();
            steerForce += normToAgent;
        }
    }

    return steerForce;
}

glm::vec2 SteeringBehaviors::Alignment(const std::vector<std::weak_ptr<TrafficCar>>& agents) {
    glm::vec2 avgHeading;

    for (const auto & agent : agents) {
        if (agent != aiComponent->GetOwner()) {
            glm::vec2 heading2D;
            heading2D.x = agent.lock()->GetTransform().GetForwardVector().x;
            heading2D.y = agent.lock()->GetTransform().GetForwardVector().z;

            avgHeading += heading2D;
        }
    }

    avgHeading /= (float)agents.size();
    avgHeading.x -= aiComponent->GetOwner().lock()->GetTransform().GetForwardVector().x;
    avgHeading.y -= aiComponent->GetOwner().lock()->GetTransform().GetForwardVector().z;

    return avgHeading;
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
