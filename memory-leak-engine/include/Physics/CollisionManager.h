#pragma once


namespace mlg {

    class CollisionManager {
    private:
        static CollisionManager* instance;

        std::vector<std::weak_ptr<class Collider>> colliders;
        std::unique_ptr<class SpacialHashGrid> spacialHashGrid;

        struct Collision {
            std::weak_ptr<Collider> collider;
            std::weak_ptr<Collider> anotherCollider;
        };

        std::vector<Collision> collisionsThisTick;

        CollisionManager();
    public:
        static void Initialize();
        static void Stop();

        static CollisionManager* GetInstance();

        static void UpdateSpacialGrid();
        static void DetectCollisions();
        static void SolveCollisions();

        static void AddCollider(const std::weak_ptr<Collider>& collider);
        static void RemoveCollider(std::weak_ptr<Collider> collider);

        static void SetBounds(const glm::vec2& start, const glm::vec2& end, const glm::ivec2& dimensions);

        static void OverlapCircle(const glm::vec2& position, float radius, std::vector<Collider*>& output);
        static void FindNear(const glm::vec2& position, float radius, std::vector<std::shared_ptr<Collider>>& result);

        static void DrawSpacialGrid();
        static void DrawSpacialGridBounds();
    };

} // mlg
