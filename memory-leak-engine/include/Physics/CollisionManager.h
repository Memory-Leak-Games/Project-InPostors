#pragma once

#include "SpacialHashGrid.h"

namespace mlg {

    class CollisionManager {
    private:
        static CollisionManager* instance;

        std::vector<std::weak_ptr<class Collider>> colliders;
        SpacialHashGrid spacialHashGrid;

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
    };

} // mlg
