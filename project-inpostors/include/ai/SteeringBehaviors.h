#pragma once

class AIComponent;
class TrafficCar;

const float waypointSeekDistance = 20;

class SteeringBehaviors {
public:
    enum SummingMethod {
        weightedAverage,
        prioritized,
        dithered
    };

private:
    enum behaviorType {
        none               = 0x00000,
        seek               = 0x00002,
        flee               = 0x00004,
        arrive             = 0x00008,
        wander             = 0x00010,
        cohesion           = 0x00020,
        separation         = 0x00040,
        alignment          = 0x00080,
        obstacleAvoidance  = 0x00100,
        wallAvoidance      = 0x00200,
        followPath         = 0x00400,
        pursuit            = 0x00800,
        evade              = 0x01000,
        interpose          = 0x02000,
        hide               = 0x04000,
        flock              = 0x08000,
        offset_pursuit     = 0x10000,
    };

    uint64_t flags;

    AIComponent* aiComponent;
    TrafficCar* trafficCar;

    enum Deceleration {
        slow = 3,
        normal = 2,
        fast = 1
    };

    Deceleration deceleration;
    SummingMethod summingMethod;

    glm::vec2 steeringForce;
    glm::vec2 target;

    float viewDistance;

    // Weights for adjusting behavior strength
    float separationWeight;
    float alignmentWeight;
    float seekWeight;
    float arriveWeight;
    float followPathWeight;

    bool BehaviorTypeOn(behaviorType bt) { return (flags & bt) == bt; }
    bool AccumulateForce(glm::vec2& sf, glm::vec2 forceToAdd);

    // Behavior declarations
    glm::vec2 Seek(glm::vec2 targetPos);
    glm::vec2 Arrive(glm::vec2 targetPos, Deceleration deceleration);
    glm::vec2 FollowPath();

    // Group behaviors
    glm::vec2 Separation(const std::vector<std::weak_ptr<TrafficCar>>& agents);

    glm::vec2 Alignment(const std::vector<std::weak_ptr<TrafficCar>>& agents);

    // Different calculation methods
    glm::vec2 CalculatePrioritized();

public:
    SteeringBehaviors(AIComponent* agent, const std::string& configPath = "res/config/ai.json");
    virtual ~SteeringBehaviors();

    glm::vec2 Calculate();

    glm::vec2 GetSteeringForce() const { return steeringForce; }

    float GetSeparationWeight() const { return separationWeight; }
    float GetAlignmentWeight() const { return alignmentWeight; }

    void SetSummingMethod(SummingMethod sm) { summingMethod = sm; }
    void SetTarget(const glm::vec2 t) { target = t; }

    // Flag checks
    void SeekOn() { flags |= seek; }
    void ArriveOn() { flags |= arrive; }
    void SeparationOn() { flags |= separation; }
    void AlignmentOn() { flags |= alignment; }
    void FollowPathOn() { flags |= followPath; }

    void SeekOff() { if(BehaviorTypeOn(seek)) flags ^= seek; }
    void ArriveOff() { if(BehaviorTypeOn(arrive)) flags ^= arrive; }
    void SeparationOff() { if(BehaviorTypeOn(separation)) flags ^= separation; }
    void AlignmentOff() { if(BehaviorTypeOn(alignment)) flags ^= alignment; }
    void FollowPathOff() { if(BehaviorTypeOn(followPath)) flags ^= followPath; }

    bool isSeekOn() { return BehaviorTypeOn(seek); }
    bool isArriveOn() { return BehaviorTypeOn(arrive); }
    bool isSeparationOn() { return BehaviorTypeOn(separation); }
    bool isAlignmentOn() { return BehaviorTypeOn(alignment); }
    bool isFollowPathOn() { return BehaviorTypeOn(followPath); }

    void LoadParameters(const std::string& path);
};
