#pragma once

namespace mlg {

    class CollisionManager {
    private:
        static CollisionManager* instance;

        std::vector<std::shared_ptr<class Collider>> colliders;

        CollisionManager() = default;
    public:
        static void Initialize();
        static void Stop();

        static CollisionManager* GetInstance();

        static void DetectCollisions();
        static void SolveCollisions();

        static void AddCollider(const std::shared_ptr<Collider>& collider);
        static void RemoveCollider(std::shared_ptr<Collider>& collider);
    };

} // mlg
