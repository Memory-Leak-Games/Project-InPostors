
#pragma once

namespace mlg {

    class CollisionManager {
    private:
        static CollisionManager* instance;

        CollisionManager() = default;
    public:
        static void Initialize();
        static void Stop();

        static CollisionManager* GetInstance();

        static void SolveCollisions();
    };

} // mlg
