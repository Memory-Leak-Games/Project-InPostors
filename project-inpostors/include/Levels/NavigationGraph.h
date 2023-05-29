#pragma once

class NavigationGraph {
public:
    struct Node {
        glm::vec2 position;
        std::vector<std::shared_ptr<Node>> connectedNodes;

        bool operator==(const Node &other) const { return position == other.position; }
        bool operator!=(const Node &other) const { return !(*this == other); }
    };

private:
    std::list<std::shared_ptr<Node>> nodes;

    std::vector<std::string> layout;
    char crossCharacter;

    float tileSize;
    glm::vec2 citySize {};
    glm::ivec2 layoutSize {};

public:
    explicit NavigationGraph(const std::string& levelPath);

    Node GetNearestNode(const glm::vec2& position);

    void DrawNodes();

private:
    void ParseLayout();
    void OptimizeNodes();
};