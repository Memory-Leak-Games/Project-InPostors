#include "ai/Path.h"

using Random = effolkronium::random_static;

Path::Path() : isLooped(false), currentWaypoint() {
}

Path::Path(std::list<glm::vec2> waypoints, bool isLooped)
    : isLooped(isLooped), waypoints(waypoints) {
    currentWaypoint = this->waypoints.begin();
}

Path::Path(float minX, float minY, float maxX,
           float maxY, bool isLooped) : isLooped(isLooped) {
    CreateBasePath(minX, minY, maxX, maxY);

    currentWaypoint = waypoints.begin();
}

Path::~Path() = default;

glm::vec2 Path::GetCurrentWaypoint() {
    return *currentWaypoint;
}

bool Path::IsPathCompleted() {
    return !(currentWaypoint != waypoints.end());
}

void Path::SetNextWaypoint() {
    MLG_ASSERT(!waypoints.empty());

    auto nextWaypoint = currentWaypoint;
    nextWaypoint++;

    if (nextWaypoint == waypoints.end())
    {
        if (isLooped) {
            currentWaypoint = waypoints.begin();
        }

        return ;
    }

    currentWaypoint++;
}

std::list<glm::vec2> Path::CreateBasePath(float minX, float minY,
                                      float maxX, float maxY) {
    waypoints.clear();

    waypoints.emplace_back(minX, minY);
    waypoints.emplace_back(maxX, minY);
    waypoints.emplace_back(maxX, maxY);
    waypoints.emplace_back(minX, maxY);

    currentWaypoint = waypoints.begin();
    return waypoints;
}

void Path::LoopOn() {
    isLooped = true;
}
void Path::LoopOff() {
    isLooped = false;
}

void Path::AddWaypoint(glm::vec2 point) {
    waypoints.push_back(point);
}

void Path::Set(std::list<glm::vec2> path) {
    waypoints = path;
    currentWaypoint = waypoints.begin();
}

void Path::Set(const Path& path) {
    waypoints = path.GetPath();
    currentWaypoint = waypoints.begin();
}

void Path::Clear() {
    waypoints.clear();
}

std::list<glm::vec2> Path::GetPath() const {
    return waypoints;
}
