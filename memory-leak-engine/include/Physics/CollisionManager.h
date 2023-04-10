#pragma once

namespace mlg {

    class CollisionManager {
    private:
        static CollisionManager* instance;

        std::vector<std::weak_ptr<class Collider>> colliders;

        struct Collision {
            std::weak_ptr<Collider> collider;
            std::weak_ptr<Collider> anotherCollider;
        };

        std::vector<Collision> collisionsThisTick;

        CollisionManager() = default;
    public:
        static void Initialize();
        static void Stop();

        static CollisionManager* GetInstance();

        static void DetectCollisions();
        static void SeparateColliders();

        static void AddCollider(std::weak_ptr<Collider> collider);
        static void RemoveCollider(std::weak_ptr<Collider> collider);
    };

} // mlg
