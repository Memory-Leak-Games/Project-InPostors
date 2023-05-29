#include "Levels/NavigationGraph.h"
#include "Core/Math.h"
#include "Rendering/Gizmos/Gizmos.h"
#include <vector>

NavigationGraph::NavigationGraph(
        const std::vector<std::string>& layout,
        char crossCharacter,
        float tileSize)
    : layout(layout), crossCharacter(crossCharacter), tileSize(tileSize) {
}

void NavigationGraph::DrawNodes() {
#ifdef DEBUG
    for (const auto& node : nodes) {
        mlg::Gizmos::DrawSphere(mlg::Math::ProjectTo3D(node.position));
    }
#endif
}

void NavigationGraph::ParseLayout() {
    for (int y = 0; y < layout.size(); ++y) {
        for (int x = 0; x < layout[y].size(); ++x) {
            if (layout[y][x] != crossCharacter)
                continue;

            Node newNode;
            newNode.position = AverageCenterLocations(x, y);
        }
    }
}

glm::vec2 NavigationGraph::AverageCenterLocations(int x, int y) {
    std::vector<glm::vec2> cornerPositions;

    // find near corners
    for (int i = -1; i < 1; ++i) {
        if (y + i > layout.size() || y + i < 0)
            continue;

        for (int j = -1; j < 1; ++j) {
            if (x + i < 0 || x + i > layout[i].size())
                continue;

            float tileX = (float) (x + j) * tileSize + tileSize / 2;
            float tileY = (float) (y + i) * tileSize + tileSize / 2;

            cornerPositions.emplace_back(tileX, tileY);
        }
    }

    // average position
    glm::vec2 result{0};
    for (const auto& corner : cornerPositions) {
        result += corner;
    }

    return result / (float) cornerPositions.size();
}
