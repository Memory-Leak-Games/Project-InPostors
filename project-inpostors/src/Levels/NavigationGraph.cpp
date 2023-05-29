#include "Levels/NavigationGraph.h"

#include <fstream>
#include <stack>

#include "Core/Math.h"
#include "Rendering/Gizmos/Gizmos.h"

using json = nlohmann::json;

NavigationGraph::NavigationGraph(const std::string& levelPath) {
    std::ifstream levelFile{levelPath};
    json levelJson = json::parse(levelFile);

    tileSize = levelJson.value("tile-size", 1.f);
    crossCharacter = levelJson.value("cross-character", 'x');
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
    for (const auto& node : nodes) {
        mlg::Gizmos::DrawSphere(
                mlg::Math::ProjectTo3D(node.position), 0.5f);
    }
#endif
}

void NavigationGraph::ParseLayout() {
    for (int y = 0; y < layout.size(); ++y) {
        for (int x = 0; x < layout[y].size(); ++x) {
            if (layout[y][x] != crossCharacter)
                continue;

            Node newNode;
            newNode.position = {layoutSize.x - x, layoutSize.y - y};
            newNode.position *= tileSize;
            newNode.position -= citySize / 2.f;
            newNode.position -= glm::vec2{0.f, 1.f} + tileSize / 2.f;

            nodes.push_back(newNode);
        }
    }

    OptimizeNodes();
}

void NavigationGraph::OptimizeNodes() {
    std::vector<Node*> nodesToRemove;
    std::unordered_set<Node*> visitedNodes;

    for (auto& nodeOne : nodes) {
        if (visitedNodes.contains(&nodeOne))
            continue;

        for (auto& nodeTwo : nodes) {
            if (&nodeOne == &nodeTwo)
                continue;

            if (glm::length(nodeOne.position - nodeTwo.position) > tileSize)
                continue;

            nodeOne.position = (nodeOne.position + nodeTwo.position) / 2.f;

            nodesToRemove.push_back(&nodeTwo);
            visitedNodes.insert(&nodeTwo);
        }
    }

    for (auto node : nodesToRemove) {
        unsigned long x = nodes.remove(*node);
    }
}