#include "Physics/SpacialHashGrid.h"

#include "Physics/Colliders/Collider.h"
#include "Core/Math.h"

namespace mlg {
    SpacialHashGrid::SpacialHashGrid(const glm::vec2& boundsStart, const glm::vec2& boundsEnd, const glm::ivec2& dimensions)
        : boundsStart(boundsStart), boundsEnd(boundsEnd), dimensions(dimensions) {
        for (int x = 0; x <= dimensions.x; ++x) {
            for (int y = 0; y <= dimensions.y; ++y) {
                hashGrid.emplace_back();
            }
        }
    }

    void SpacialHashGrid::Insert(const std::shared_ptr<Collider>& client) {
        const glm::vec2 clientPosition = client->GetPosition();
        const float clientRadius = client->GetRadius();

        glm::ivec2 gridCoordMin = CalculateCellIndex(clientPosition - clientRadius * glm::vec2{1.f});
        glm::ivec2 gridCoordMax = CalculateCellIndex(clientPosition + clientRadius * glm::vec2{1.f});

        client->minCoord = gridCoordMin;
        client->maxCoord = gridCoordMax;

        for (int x = gridCoordMin.x; x <= gridCoordMax.x; ++x) {
            for (int y = gridCoordMin.y; y <= gridCoordMax.y; ++y) {
                int key = x + dimensions.x * y;
                hashGrid[key].insert(client);
            }
        }
    }

    glm::ivec2 SpacialHashGrid::CalculateCellIndex(glm::vec2 position) const {
        glm::vec2 positionInBounds;
        positionInBounds.x = Math::Sat((position.x - this->boundsStart.x) / (this->boundsEnd.x - this->boundsStart.x));
        positionInBounds.y = Math::Sat((position.y - this->boundsStart.y) / (this->boundsEnd.y - this->boundsStart.y));

        glm::ivec2 result;
        result.x = glm::floor(positionInBounds.x * (float) (dimensions.x - 1));
        result.y = glm::floor(positionInBounds.y * (float) (dimensions.y - 1));

        return result;
    }

    void SpacialHashGrid::Update(const std::shared_ptr<Collider>& client) {
        RemoveClient(client);
        Insert(client);
    }

    void SpacialHashGrid::FindNear(const glm::vec2& position, float radius, std::unordered_set<std::shared_ptr<Collider>>& result) {
        glm::ivec2 gridCoordMin = CalculateCellIndex(position - radius * glm::vec2{1.f});
        glm::ivec2 gridCoordMax = CalculateCellIndex(position + radius * glm::vec2{1.f});

        for (int x = gridCoordMin.x; x <= gridCoordMax.x; ++x) {
            for (int y = gridCoordMin.y; y <= gridCoordMax.y; ++y) {
                int key = x + dimensions.x * y;

                result.insert(hashGrid[key].begin(), hashGrid[key].end());
            }
        }
    }

    void SpacialHashGrid::RemoveClient(const std::shared_ptr<Collider>& client) {
        glm::ivec2 gridCoordMin = client->minCoord;
        glm::ivec2 gridCoordMax = client->maxCoord;

        for (int x = gridCoordMin.x; x <= gridCoordMax.x; ++x) {
            for (int y = gridCoordMin.y; y <= gridCoordMax.y; ++y) {
                int key = x + dimensions.x * y;
                hashGrid[key].erase(client);
            }
        }
    }

    void SpacialHashGrid::AddClient(const std::shared_ptr<Collider>& client) {
        Insert(client);
    }


}// namespace mlg