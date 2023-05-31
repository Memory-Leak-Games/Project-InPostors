#pragma once

class NavigationGraph {
public:
    struct Node {
        size_t id;
        glm::vec2 position;
        glm::ivec2 layoutPosition;
        std::unordered_set<std::shared_ptr<Node>> connectedNodes;

        bool operator==(const Node &other) const { return position == other.position; }
        bool operator!=(const Node &other) const { return !(*this == other); }
    };

    using NodeSharedPtr = std::shared_ptr<Node>;

private:
    std::list<NodeSharedPtr> nodes;

    std::vector<std::string> layout;
    char crossCharacter;
    char roadCharacter;

    float tileSize;
    glm::vec2 citySize {};
    glm::ivec2 layoutSize {};

    size_t idCounter = 1;

public:
    explicit NavigationGraph(const std::string& levelPath);

    const Node& GetNearestNode(const glm::vec2& position);
    void DrawNodes();

private:
    void ParseLayout();
    void OptimizeNodes();
    void FindConnections();
    void AddConnectionWhenParametersMeet(
            const NodeSharedPtr& nodeOne,
            const NodeSharedPtr& nodeTwo);

    bool TraceConnection(glm::ivec2 start, glm::ivec2 end);
    NavigationGraph::Node* HasDirection(Node& nodeOne, Node& nodeTwo);
    glm::ivec2 CalculateLayoutDirection(const Node& nodeOne, const Node& nodeTwo);

};