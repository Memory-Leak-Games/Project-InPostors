#pragma once

class NavigationGraph {
public:
    struct Node {
        glm::vec2 position;
        glm::ivec2 layoutPosition;
        std::unordered_set<std::shared_ptr<Node>> connectedNodes;

        bool operator==(const Node &other) const { return position == other.position; }
        bool operator!=(const Node &other) const { return !(*this == other); }
    };

private:
    std::list<std::shared_ptr<Node>> nodes;

    std::vector<std::string> layout;
    char crossCharacter;
    char roadCharacter;

    float tileSize;
    glm::vec2 citySize {};
    glm::ivec2 layoutSize {};

public:
    explicit NavigationGraph(const std::string& levelPath);

    Node GetNearestNode(const glm::vec2& position);

    std::list<glm::vec2> CreatePath();

    void DrawNodes();

private:
    void ParseLayout();
    void OptimizeNodes();
    void FindConnections();

    bool TraceConnection(glm::ivec2 start, glm::ivec2 end);
};