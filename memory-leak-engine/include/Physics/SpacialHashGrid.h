#pragma once

namespace mlg {

    class SpacialHashGrid {
    private:
        glm::vec2 boundsStart;
        glm::vec2 boundsEnd;
        glm::ivec2 dimensions;

        std::vector<std::vector<std::shared_ptr<class Collider>>> hashGrid;

        // used to deduplicate clients in FindNear
        uint32_t queryIDs = 0;

    public:
        SpacialHashGrid(const glm::vec2& boundsStart, const glm::vec2& boundsEnd, const glm::ivec2& dimensions);

        void AddClient(const std::shared_ptr<Collider>& client);
        void RemoveClient(const std::shared_ptr<Collider>& client);

        void FindNear(const glm::vec2& position, float radius, std::vector<std::shared_ptr<Collider>>& result);
        void Update(const std::shared_ptr<Collider>& client);

        void DebugDraw();
        void DebugDrawBounds();
    private:
        void Insert(const std::shared_ptr<Collider>& client);
        [[nodiscard]] glm::ivec2 CalculateCellIndex(const glm::vec2& position) const;

        int GetHashIndex(const glm::ivec2& position);

    };
}// namespace mlg
