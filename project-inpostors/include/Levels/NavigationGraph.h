#pragma once

#include <string>
#include <vector>

class NavigationGraph {
public:
    struct Node {
        glm::vec2 position;
        std::vector<std::shared_ptr<NavigationGraph>> connectedNodes;
    };

private:
    std::vector<Node> nodes;

    std::vector<std::string> layout;
    char crossCharacter;
    float tileSize;

public:
    explicit NavigationGraph(
            const std::vector<std::string>& layout,
            char crossCharacter,
            float tileSize);

    Node GetNearestNode(const glm::vec2& position);

    void DrawNodes();

private:
    void ParseLayout();
    glm::vec2 AverageCenterLocations(int x, int y);

};