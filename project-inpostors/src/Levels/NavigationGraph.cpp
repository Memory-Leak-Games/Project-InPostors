#include "Levels/NavigationGraph.h"

#include <fstream>
#include <glm/fwd.hpp>

#include "Core/Math.h"
#include "Rendering/Gizmos/Gizmos.h"

using json = nlohmann::json;

NavigationGraph::NavigationGraph(const std::string& levelPath) {
    std::ifstream levelFile{levelPath};
    json levelJson = json::parse(levelFile);

    tileSize = levelJson.value("tile-size", 1.f);
    crossCharacter = levelJson.value("cross-character", 'x');
    roadCharacter = levelJson.value("road-character", '#');
    layout = levelJson["navigation"];

    layoutSize.y = layout.size();

    for (const auto& row : layout) {
        layoutSize.x = std::max(layoutSize.x, (int) row.size());
    }

    citySize.x = (float) layoutSize.x * tileSize;
    citySize.y = (float) layoutSize.y * tileSize;

    ParseLayout();
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

            nodes.push_back(newNode);
        }
    }

    OptimizeNodes();
    FindConnections();
}

void NavigationGraph::OptimizeNodes() {
    std::vector<std::shared_ptr<Node>> nodesToRemove;
    std::unordered_set<Node*> visitedNodes;

    for (auto& nodeOne : nodes) {
        if (visitedNodes.contains(nodeOne.get()))
            continue;

        for (auto& nodeTwo : nodes) {
            if (&nodeOne == &nodeTwo)
                continue;

            if (glm::length(nodeOne->position - nodeTwo->position) > tileSize)
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
            glm::ivec2 manhattanDistance = nodeOne->layoutPosition - nodeTwo->layoutPosition;
            if (manhattanDistance.x == 0 && manhattanDistance.y == 0)
                continue;

            if (manhattanDistance.x != 0 && manhattanDistance.y != 0)
                continue;

            if (TraceConnection(nodeOne->layoutPosition, nodeTwo->layoutPosition))
                continue;   

            nodeOne->connectedNodes.insert(nodeTwo);
        }
    }
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

std::list<std::shared_ptr<NavigationGraph::Node>> NavigationGraph::GetNodes() {
    return nodes;
}
