#pragma once

class AIComponent;
class TrafficCar;
class Path;
class NavigationGraph;

class SteeringBehaviors {
public:
    enum SummingMethod {
        weightedAverage,
        prioritized,
        dithered
    };

private:
    enum BehaviorType {
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
    std::unique_ptr<Path> path;
    std::shared_ptr<NavigationGraph> navigationGraph;

    enum Deceleration {
        slow = 3,
        normal = 2,
        fast = 1
    };

    Deceleration deceleration;
    SummingMethod summingMethod;

    glm::vec2 steeringForce;
    glm::vec2 target;

    // Waypoint seeking distance
    float waypointSeekingDistance;

    // Weights for adjusting behavior strength
    float separationWeight;
    float alignmentWeight;
    float seekWeight;
    float arriveWeight;
    float followPathWeight;
    float evadeWeight;

    bool BehaviorTypeOn(BehaviorType bt) const;
    bool AccumulateForce(glm::vec2& sf, glm::vec2 forceToAdd);

    // Behavior declarations
    glm::vec2 Seek(glm::vec2 targetPos);
    glm::vec2 Evade();
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

    glm::vec2 GetSteeringForce() const;

    float GetSeparationWeight() const;
    float GetAlignmentWeight() const;

    std::list<glm::vec2> GetPath() const;
    glm::vec2 GetCurrentWaypoint() const;
    std::shared_ptr<NavigationGraph> GetNavigationGraph() const;

    void SetNavigationGraph(std::shared_ptr<NavigationGraph> navGraph);
    void CreatePath(int numberOfNodes);
    void SetPath(std::list<glm::vec2> newPath);
    void CreateBasePath(float minX, float minY, float maxX, float maxY) const;

    void SetSummingMethod(SummingMethod sm);
    void SetTarget(const glm::vec2 t);

    // Flag checks
    void SeekOn();
    void ArriveOn();
    void EvadeOn();
    void SeparationOn();
    void AlignmentOn();
    void FollowPathOn();
    void TrafficDriveOn();

    void SeekOff();
    void ArriveOff();
    void EvadeOff();
    void SeparationOff();
    void AlignmentOff();
    void FollowPathOff();
    void TrafficDriveOff();

    bool IsSeekOn();
    bool IsArriveOn();
    bool IsEvadeOn();
    bool IsSeparationOn();
    bool IsAlignmentOn();
    bool IsFollowPathOn();

    void LoadParameters(const std::string& path);
};
