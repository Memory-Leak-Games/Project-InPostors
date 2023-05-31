#include "Levels/NavigationGraph.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <memory>
#include <spdlog/spdlog.h>

#include "Core/Math.h"
#include "Rendering/Gizmos/Gizmos.h"

using json = nlohmann::json;

NavigationGraph::NavigationGraph(const std::string& levelPath) {
    std::ifstream levelFile{levelPath};
    json levelJson = json::parse(levelFile);

    tileSize = levelJson.value("tile-size", 1.f);
    crossCharacter = levelJson.value("cross-character", "x")[0];
    roadCharacter = levelJson.value("road-character", "#")[0];
    layout = levelJson["navigation"];

    layoutSize.y = layout.size();

    for (const auto& row : layout) {
        layoutSize.x = std::max(layoutSize.x, (int) row.size());
    }

    citySize.x = (float) layoutSize.x * tileSize;
    citySize.y = (float) layoutSize.y * tileSize;

    ParseLayout();
}

const NavigationGraph::Node& NavigationGraph::GetNearestNode(const glm::vec2& position) {
    auto byDistance =
            [&position](const NodeSharedPtr& nodeOne, const NodeSharedPtr& nodeTwo) {
                float distanceOne = glm::length(position - nodeOne->position);
                float distanceTwo = glm::length(position - nodeTwo->position);
                return distanceOne > distanceTwo;
            };

    auto nearestNode = std::max_element(
            nodes.begin(),
            nodes.end(),
            byDistance);

    return **nearestNode;
}

const std::list<NavigationGraph::NodeSharedPtr>& NavigationGraph::GetAllNodes() {
    return nodes;
}

void NavigationGraph::DrawNodes() {
#ifdef DEBUG
    // draw points
    for (const auto& node : nodes) {
        mlg::Gizmos::DrawSphere(
                mlg::Math::ProjectTo3D(node->position), 0.5f);
    }

    // draw connections
    for (const auto& node : nodes) {
        for (const auto& connectedNode : node->connectedNodes) {
            mlg::Gizmos::DrawLine(
                    mlg::Math::ProjectTo3D(node->position),
                    mlg::Math::ProjectTo3D(connectedNode->position),
                    mlg::RGBA::green);
        }
    }
#endif
}

void NavigationGraph::ParseLayout() {
    for (int y = 0; y < layout.size(); ++y) {
        for (int x = 0; x < layout[y].size(); ++x) {
            if (layout[y][x] != crossCharacter)
                continue;

            auto newNode = std::make_shared<Node>();
            newNode->position = {layoutSize.x - x, layoutSize.y - y};
            newNode->position *= tileSize;
            newNode->position -= citySize / 2.f;
            newNode->position -= glm::vec2{0.f, 1.f} + tileSize / 2.f;

            newNode->layoutPosition = {x, y};
            newNode->id = idCounter;
            idCounter++;

            nodes.push_back(newNode);
        }
    }

    OptimizeNodes();
    FindConnections();

    SPDLOG_INFO("Found nodes: {}", nodes.size());
}

void NavigationGraph::OptimizeNodes() {
    std::vector<NodeSharedPtr> nodesToRemove;
    std::unordered_set<Node*> visitedNodes;

    for (auto& nodeOne : nodes) {
        if (visitedNodes.contains(nodeOne.get()))
            continue;

        for (auto& nodeTwo : nodes) {
            if (&nodeOne == &nodeTwo)
                continue;

            if (glm::length(nodeOne->position - nodeTwo->position) > tileSize * std::sqrt(2.f))
                continue;

            nodeOne->position = (nodeOne->position + nodeTwo->position) / 2.f;

            nodesToRemove.push_back(nodeTwo);
            visitedNodes.insert(nodeTwo.get());
        }
    }

    for (const auto& node : nodesToRemove) {
        unsigned long x = nodes.remove(node);
    }
}

void NavigationGraph::FindConnections() {
    for (auto& nodeOne : nodes) {
        for (auto& nodeTwo : nodes) {
            AddConnectionWhenParametersMeet(nodeOne, nodeTwo);
        }
    }
}

void NavigationGraph::AddConnectionWhenParametersMeet(
        const NodeSharedPtr& nodeOne,
        const NodeSharedPtr& nodeTwo) {
    glm::ivec2 manhattanDistance = nodeOne->layoutPosition - nodeTwo->layoutPosition;

    if (manhattanDistance.x == 0 && manhattanDistance.y == 0)
        return;

    if (manhattanDistance.x != 0 && manhattanDistance.y != 0)
        return;

    if (TraceConnection(nodeOne->layoutPosition, nodeTwo->layoutPosition))
        return;

    Node* anotherNodeWithSameDirection = HasNodeWithSameDirection(*nodeOne, *nodeTwo);
    if (anotherNodeWithSameDirection != nullptr) {
        glm::vec2 distanceToTwo = nodeTwo->layoutPosition - nodeOne->layoutPosition;
        glm::vec2 distanceToAnother =
                anotherNodeWithSameDirection->layoutPosition - nodeOne->layoutPosition;


        if (glm::length(distanceToAnother) < glm::length(distanceToTwo))
            return;

        // replace longer connection
        auto sameNodePredicate =
                [anotherNodeWithSameDirection](const NodeSharedPtr& item) {
                    return anotherNodeWithSameDirection == item.get();
                };
        std::erase_if(nodeOne->connectedNodes, sameNodePredicate);
    }

    nodeOne->connectedNodes.insert(nodeTwo);

    SPDLOG_DEBUG("{} -> {}", nodeOne->id, nodeTwo->id);
}

bool NavigationGraph::TraceConnection(glm::ivec2 start, glm::ivec2 end) {
    int minX = std::min(start.x, end.x);
    int maxX = std::max(start.x, end.x);

    for (int i = minX; i < maxX; ++i) {
        char character = layout[start.y][i];
        if (character != roadCharacter && character != crossCharacter)
            return true;
    }

    int minY = std::min(start.y, end.y);
    int maxY = std::max(start.y, end.y);

    for (int i = minY; i < maxY; ++i) {
        char character = layout[i][start.x];
        if (character != roadCharacter && character != crossCharacter)
            return true;
    }

    return false;
}

NavigationGraph::Node* NavigationGraph::HasNodeWithSameDirection(Node& nodeOne, Node& nodeTwo) {
    glm::ivec2 directionToTwo = CalculateLayoutDirection(nodeOne, nodeTwo);

    for (const auto& connectedNode : nodeOne.connectedNodes) {
        glm::ivec2 directionToConnected = CalculateLayoutDirection(nodeOne, *connectedNode);

        if (directionToTwo == directionToConnected)
            return connectedNode.get();
    }

    return nullptr;
}

glm::ivec2 NavigationGraph::CalculateLayoutDirection(const Node& nodeOne, const Node& nodeTwo) {
    glm::ivec2 direction = nodeTwo.layoutPosition - nodeOne.layoutPosition;
    direction.x = std::abs(direction.x) > 0 ? 1 : 0;
    direction.y = std::abs(direction.y) > 0 ? 1 : 0;

    return direction;
}
