#pragma once

namespace mlg {

    class AI {
    public:
        eventpp::CallbackList<void ()> OnAIUpdate;

    private:
        static AI* instance;

        float timeAccumulator = 0;

        std::vector<std::weak_ptr<class Entity>> entities;

        AI() = default;

    public:
        static void Initialize();
        static void Stop();
        static AI* GetInstance();

        static void AddEntity(std::weak_ptr<Entity> entity);
        static void RemoveEntity(std::weak_ptr<Entity> entity);

        static void TickAITimeStep();
    };

}// namespace mlg
