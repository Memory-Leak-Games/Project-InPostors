#pragma once

class Path {
private:
    std::list<glm::vec2> waypoints;
    std::list<glm::vec2>::iterator currentWaypoint;
    bool isLooped;

public:
    Path();
    Path(std::list<glm::vec2> waypoints, bool isLooped);
    Path(float minX, float minY, float maxX, float maxY, bool isLooped);
    ~Path();

    glm::vec2 GetCurrentWaypoint();
    bool IsPathCompleted();
    void SetNextWaypoint();

    std::list<glm::vec2> CreateBasePath(float minX, float minY, float maxX, float maxY);

    void LoopOn();
    void LoopOff();

    void AddWaypoint(glm::vec2 point);

    void Set(std::list<glm::vec2> path);
    void Set(const Path& path);

    void Clear();

    std::list<glm::vec2> GetPath() const;
};
