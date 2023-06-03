#include <spdlog/spdlog.h>
#include <utility>

#include <fstream>
#include <nlohmann/json.hpp>

#include "ai/AIComponent.h"
#include "ai/Path.h"
#include "ai/SteeringBehaviors.h"
#include "ai/TrafficCar.h"

#include "Levels/NavigationGraph.h"

#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"

using json = nlohmann::json;
using Random = effolkronium::random_static;

SteeringBehaviors::SteeringBehaviors(AIComponent* agent, const std::string& configPath)
    : aiComponent(agent), flags(0), deceleration(fast), summingMethod(prioritized) {
    LoadParameters(configPath);

    //TODO: Replace this with Path from level
    //    std::list<glm::vec2> waypoints;
    //
    //    for (const auto& node : navigationGraph->GetNodes()) {
    //        waypoints.push_back(node->position);
    //    }

    //    path = new Path(waypoints, true);
    path = new Path();
}

SteeringBehaviors::~SteeringBehaviors() = default;

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
    steeringForce = {0, 0};

    switch (summingMethod) {
        case weightedAverage:
            //TODO: Implement weighted average in the future
            steeringForce = {0, 0};
            break;
        case prioritized:
            steeringForce = CalculatePrioritized();
            break;
        case dithered:
            //TODO: Implement dithering in the future
            steeringForce = {0, 0};
            break;
        default:
            steeringForce = {0, 0};
    }

    return steeringForce;
}

glm::vec2 SteeringBehaviors::CalculatePrioritized() {
    glm::vec2 force;
    std::vector<std::weak_ptr<TrafficCar>> cars = mlg::EntityManager::FindAllByType<TrafficCar>();

    // remove this car from cars vector
    cars.erase(std::remove_if(cars.begin(), cars.end(), [this](const std::weak_ptr<TrafficCar>& car) {
        return car.lock()->GetId() == aiComponent->GetOwner().lock()->GetId();
    }), cars.end());


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
        force = Seek(target) * seekWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(arrive)) {
        force = Arrive(target, deceleration) * arriveWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    if (BehaviorTypeOn(followPath)) {
        force = FollowPath() * followPathWeight;

        if (!AccumulateForce(steeringForce, force))
            return steeringForce;
    }

    return steeringForce;
}

glm::vec2 SteeringBehaviors::Seek(glm::vec2 targetPos) {
    glm::vec2 desiredVelocity = glm::normalize(targetPos - aiComponent->GetPosition()) * aiComponent->GetMaxSpeed();

    glm::vec2 velocity2D = aiComponent->GetLinearVelocity();
    return (desiredVelocity - velocity2D);
}

glm::vec2 SteeringBehaviors::Arrive(glm::vec2 targetPos, Deceleration dec) {
    glm::vec2 toTarget = targetPos - aiComponent->GetPosition();

    float distance = toTarget.length();

    if (distance > 0) {
        const float decelerationTweaker = 0.3;

        float speed = distance / ((float) dec * decelerationTweaker);
        speed = fmin(speed, aiComponent->GetMaxSpeed());

        glm::vec2 desiredVelocity = toTarget * speed / distance;
        return (desiredVelocity - aiComponent->GetLinearVelocity());
    }

    return {0, 0};
}

glm::vec2 SteeringBehaviors::Separation(const std::vector<std::weak_ptr<TrafficCar>>& agents) {
    glm::vec2 steerForce {};

    for (const auto& agent : agents) {
        if (agent != aiComponent->GetOwner()) {
            glm::vec2 toAgent = aiComponent->GetPosition() - agent.lock()->GetComponentByClass<AIComponent>().lock()->GetPosition();

            if (glm::length(toAgent) > aiComponent->GetViewDistance())
                continue;

            SPDLOG_INFO("Separate");
            glm::vec2 normToAgent = glm::normalize(toAgent);
            normToAgent.x /= glm::length(toAgent);
            normToAgent.y /= glm::length(toAgent);
//            normToAgent.x *= 5;
//            normToAgent.y *= 5;
            steerForce += normToAgent;
        }
    }

    return steerForce;
}

glm::vec2 SteeringBehaviors::Alignment(const std::vector<std::weak_ptr<TrafficCar>>& agents) {
    if (agents.empty())
        return {0, 0};

    glm::vec2 avgHeading {};

    for (const auto& agent : agents) {
        if (agent != aiComponent->GetOwner()) {
            glm::vec2 heading2D;
            heading2D.x = agent.lock()->GetTransform().GetForwardVector().x;
            heading2D.y = agent.lock()->GetTransform().GetForwardVector().z;

            avgHeading += heading2D;
        }
    }

    avgHeading /= (float) agents.size();
    avgHeading.x -= aiComponent->GetOwner().lock()->GetTransform().GetForwardVector().x;
    avgHeading.y -= aiComponent->GetOwner().lock()->GetTransform().GetForwardVector().z;

    return avgHeading;
}

glm::vec2 SteeringBehaviors::FollowPath() {
    if (glm::distance2(path->GetCurrentWaypoint(), aiComponent->GetPosition()) < sqrt(waypointSeekDistance)) {
        path->SetNextWaypoint();
    }

    if (!path->IsPathCompleted()) {
        return Seek(path->GetCurrentWaypoint());
    } else {
        return Arrive(path->GetCurrentWaypoint(), normal);
    }
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

void SteeringBehaviors::SetNavigationGraph(std::shared_ptr<NavigationGraph> navGraph) {
    navigationGraph = navGraph;
}

// TODO: this is hacky approach. So if you have better idea, implement it here. 
// TODO: It's good idea to keep visited nodes and try to avoid them (until car
//       visits all)
void SteeringBehaviors::CreatePath(int numberOfNodes) {
    std::list<glm::vec2> waypoints;

    const NavigationGraph::Node* currentNode =
            &navigationGraph->GetNearestNode(aiComponent->GetPosition());

    const NavigationGraph::Node* previousNode = currentNode;

    // create path
    for (int i = 0; i < numberOfNodes; ++i) {
        waypoints.push_back(currentNode->position);

        const NavigationGraph::Node* nextNode;
        do {
            nextNode = Random::get(currentNode->connectedNodes)->get();

            if (currentNode->connectedNodes.size() <= 1)
                break;

        } while (nextNode->id == previousNode->id);

        previousNode = currentNode;
        currentNode = nextNode;
    }

    // create path loop
    auto waypointRIt = waypoints.rend();

    // skip first element
    waypointRIt++;
    waypointRIt++;

    for (int i = 0; i < numberOfNodes - 2; ++i) {
        waypoints.push_back(*waypointRIt);
        waypointRIt++;
    }

    SetPath(waypoints);
    path->LoopOn();
}

void SteeringBehaviors::SetPath(std::list<glm::vec2> newPath) {
    path->Set(newPath);
    path->LoopOn();
}

void SteeringBehaviors::CreateBasePath(float minX, float minY, float maxX, float maxY) const {
    path->CreateBasePath(minX, minY, maxX, maxY);
}

glm::vec2 SteeringBehaviors::GetSteeringForce() const {
    return steeringForce;
}

float SteeringBehaviors::GetSeparationWeight() const {
    return separationWeight;
}

float SteeringBehaviors::GetAlignmentWeight() const {
    return alignmentWeight;
}

std::list<glm::vec2> SteeringBehaviors::GetPath() const {
    return path->GetPath();
}

glm::vec2 SteeringBehaviors::GetCurrentWaypoint() const {
    return path->GetCurrentWaypoint();
}

std::shared_ptr<NavigationGraph> SteeringBehaviors::GetNavigationGraph() const {
    return navigationGraph;
}

bool SteeringBehaviors::BehaviorTypeOn(BehaviorType bt) const {
    return (flags & bt) == bt;
}

void SteeringBehaviors::SetSummingMethod(SummingMethod sm) {
    summingMethod = sm;
}

void SteeringBehaviors::SetTarget(const glm::vec2 t) {
    target = t;
}

void SteeringBehaviors::SeekOn() {
    flags |= seek;
}

void SteeringBehaviors::ArriveOn() {
    flags |= arrive;
}

void SteeringBehaviors::SeparationOn() {
    flags |= separation;
}

void SteeringBehaviors::AlignmentOn() {
    flags |= alignment;
}

void SteeringBehaviors::FollowPathOn() {
    flags |= followPath;
}

void SteeringBehaviors::TrafficDriveOn() {
    FollowPathOn();
    AlignmentOn();
    SeparationOn();
}

void SteeringBehaviors::SeekOff() {
    if (BehaviorTypeOn(seek))
        flags ^= seek;
}

void SteeringBehaviors::ArriveOff() {
    if (BehaviorTypeOn(arrive))
        flags ^= arrive;
}

void SteeringBehaviors::SeparationOff() {
    if (BehaviorTypeOn(separation))
        flags ^= separation;
}

void SteeringBehaviors::AlignmentOff() {
    if (BehaviorTypeOn(alignment))
        flags ^= alignment;
}

void SteeringBehaviors::FollowPathOff() {
    if (BehaviorTypeOn(followPath))
        flags ^= followPath;
}

void SteeringBehaviors::TrafficDriveOff() {
    FollowPathOff();
    AlignmentOff();
    SeparationOff();
}

bool SteeringBehaviors::IsSeekOn() {
    return BehaviorTypeOn(seek);
}

bool SteeringBehaviors::IsArriveOn() {
    return BehaviorTypeOn(arrive);
}

bool SteeringBehaviors::IsSeparationOn() {
    return BehaviorTypeOn(separation);
}

bool SteeringBehaviors::IsAlignmentOn() {
    return BehaviorTypeOn(alignment);
}

bool SteeringBehaviors::IsFollowPathOn() {
    return BehaviorTypeOn(followPath);
}
