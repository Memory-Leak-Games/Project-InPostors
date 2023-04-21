#pragma once

namespace mlg {

    class SpacialHashGrid {
    private:
        glm::vec2 boundsStart;
        glm::vec2 boundsEnd;
        glm::ivec2 dimensions;

        std::unordered_map<int, std::unordered_set<std::shared_ptr<class Collider>>> hashGrid;

    public:
        SpacialHashGrid(const glm::vec2& boundsStart, const glm::vec2& boundsEnd, const glm::ivec2& dimensions);

        void AddClient(const std::shared_ptr<Collider>& client);
        void RemoveClient(const std::shared_ptr<Collider>& client);

        void FindNear(const glm::vec2& position, float radius, std::unordered_set<std::shared_ptr<Collider>>& result);
        void Update(const std::shared_ptr<Collider>& client);

    private:
        void Insert(const std::shared_ptr<Collider>& client);
        [[nodiscard]] glm::ivec2 CalculateCellIndex(glm::vec2 position) const;
    };
}// namespace mlg
